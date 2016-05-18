#include "cc/parser.hpp"

#include "mn_hash.hpp"
#include "ast/cond.hpp"
#include "ast/atoms.hpp"
#include "ast/defs.hpp"
#include "ast/builtins.hpp"
#include "dt/str_view.hpp"
#include "dbg/lex.hpp"
#include "dbg/dt.hpp"
#include "dbg/sym.hpp"
#include "cc/type_deducer.hpp"
#include "echo.hpp"
#include <cstring>
#include <vector>

#include "dbg/lex.hpp"

using namespace cc;
using namespace lex;
using namespace ast;
using namespace dt;
using namespace sym;

int length(TokenStream toks) {
  int size = 0;

  while (!toks.NextToken().IsEof()) {
    size += 1;
  }

  return size;
}

std::vector<Token> eval_tokens(TokenStream& toks, uint min_count, uint max_count) {
  std::vector<Token> items;
  items.reserve(min_count);

  while (!toks.NextToken().IsEof()) {
    if (items.size() == max_count) {
      throw "too much args!";
    }
    items.push_back(toks.CurrentToken());
  }

  if (items.size() < min_count) {
    throw "too few args!";
  }

  return items;
}

std::vector<Token> eval_tokens(TokenStream toks, int count) {
  return eval_tokens(toks, count, count);
}

sym::Type Parser::TypeByName(dt::StrView name) const {
  using namespace mn_hash;

  switch (encode9(name.Data(), name.Len())) {
  case encode9("int"): return Type::Int();
  case encode9("real"): return Type::Real();
  case encode9("num"): return Type::Num();
  default:
    auto type_info = module.Struct(name);
    if (type_info) {
      return type_info->type;
    } else {
      throw "unknown type";
    }
  }
}

TranslationUnit Parser::Run(const TopLevel& top) {
  Parser self{top};
  auto tu = self.Parse();
  tu.module = self.module; // #FIXME: is this causes whole module to copy?

  return tu;
}

Parser::Parser(const TopLevel& top): top{top} {}

TranslationUnit Parser::Parse() {
  for (lex::TokenStream s : top.structs) {
    ParseDefStruct(s);
  }

  for (lex::TokenStream global : top.globals) {
    ParseGlobal(global);
  }

  for (lex::TokenStream func : top.funcs) {
    ParseSignature(func);
  }

  for (lex::Token expr : top.exprs) {
    ParseExpr(expr);
  }

  /*
  for (lex::TokenStream func : top.funcs) {
    dbg::dump_token(func.CurrentToken());
  }*/

  /*
  sym::Func* fn = module.Func("sum");
  printf("arity: %d\n", fn->Arity());
  for (sym::Param param : fn->Params()) {
    dbg::dump(param.type);
  }
  dbg::dump(fn->ret_type);*/

  return result;
}

void Parser::ParseDefStruct(TokenStream& toks) {
  dt::StrView name = toks.NextToken();
  std::vector<sym::Param> attrs;

  while (!toks.NextToken().IsEof()) {
    auto tok = toks.CurrentToken();

    if (tok.IsList()) {
      lex::TokenStream typed_list{tok};
      auto type = TypeByName(typed_list.NextToken());

      while (!typed_list.NextToken().IsEof()) {
        attrs.push_back(Param{typed_list.CurrentToken(), type});
      }
    } else {
      attrs.push_back(Param{tok, Type::Any()});
    }
  }

  // #FIXME: return value (type_id) unused
  // module.DefineStruct(name, new Struct{name, attrs});
  module.DefineStruct(name, std::move(attrs));
  result.structs.push_back(name);
}

void Parser::ParseGlobal(TokenStream& args) {
  dt::StrView name = args.NextToken();
  auto expr = ParseToken(args.NextToken());
  auto ty = new sym::Type{TypeDeducer::Run(expr)};

  result.globals.push_back(new DefVar{name, expr, ty});
  module.DefineGlobalSymbol(name, ty);
}

void Parser::ParseExpr(Token& tok) {
  if (tok.IsList()) {
    result.exprs.push_back(ParseList(tok));
  } else {
    result.exprs.push_back(ParseToken(tok));
  }
}

void Parser::ParseSignature(TokenStream& toks) {
  lex::TokenStream signature{toks.NextToken()};
  auto name = signature.NextToken();

  module.CreateScopeLevel();

  std::vector<sym::Param> params;
  while (!signature.NextToken().IsEof()) {
    auto tok = signature.CurrentToken();

    if (tok.IsList()) {
      lex::TokenStream typed_list{tok};
      auto type = TypeByName(typed_list.NextToken());

      while (!typed_list.NextToken().IsEof()) {
        dt::StrView param_name = typed_list.CurrentToken();
        params.push_back(Param{param_name, type});
        module.DefineLocal(param_name, type);
      }
    } else {
      params.push_back(Param{tok, sym::Type::Any()});
      module.DefineLocal(tok, sym::Type::Any());
    }
  }

  std::vector<Node*> exprs;
  while (!toks.NextToken().IsEof()) {
    exprs.push_back(ParseToken(toks.CurrentToken()));
  }

  module.DropScopeLevel();

  auto func = new sym::Func{
    name, std::move(params), std::move(exprs), TypeDeducer::Run(exprs.back())
  };
  module.DefineFunc(name, func);
  result.funcs.push_back(name);
}

Node* Parser::ParseToken(Token tok) {
  switch (tok.Tag()) {
  case Token::INT:
    return new Int{tok};
  case Token::REAL:
    return new Real{tok};
  case Token::STR:
    return new Str{tok};
  case Token::WORD:
    return new Var{tok, module.Symbol(tok)};
  case Token::LIST:
    return ParseList(tok);

  default:
    throw "not implemented";
  }
}

Node* Parser::ParseList(Token tok) {
  using namespace mn_hash;

  TokenStream list{tok};

  if (list.NextToken().IsWord()) {
    auto name_tok = list.CurrentToken();
    auto name_hash = encode9(name_tok.Data(), name_tok.Len());

    switch (name_hash) {
    case encode9("+"): return ParseSum(list);
    case encode9("-"): return ParseSub(list);
    case encode9("set!"): return ParseSet(list);
    case encode9("def"): return ParseDef(list);
    case encode9("if"): return ParseIf(list);
    // case encode9("for"): return ParseFor(list);
    case encode9("struct"): return ParseStruct(list);
    case encode9("'"): return ParseQuote(list);
    case encode9("get"): return ParseGet(list);
    case encode9("."): return ParseAttrAccess(list);

    default:
      return ParseFuncCall(name_tok, list);
    }

  } else {
    throw "car(list) != symbol";
  }
}

/*
ast::Node* Parser::ParseFor(lex::TokenStream& toks) {
  auto inductor = toks.NextToken();
  auto iterator = ParseToken(toks.NextToken());
  auto loop_expr = ParseToken(toks.NextToken());


}*/

ast::Node* Parser::ParseFuncCall(lex::Token& name, lex::TokenStream& args) {
  sym::Func* func = module.Func(name);
  if (func) {
    std::vector<Node*> nodes;
    for (int i = 0; i < func->Arity(); ++i) {
      nodes.push_back(ParseToken(args.NextToken()));
    }
    if (!args.NextToken().IsEof()) {
      throw "too many args";
    }

    return new FuncCall{func, std::move(nodes)};
  } else {
    throw "called undefined function";
  }
}

Node* Parser::ParseSum(TokenStream& toks) {
  return new Sum{std::move(CollectParsed(toks))};
}

Node* Parser::ParseSub(TokenStream& toks) {
  return new Sub{std::move(CollectParsed(toks))};
}

// (set! obj val)
// (set! obj attr val)
Node* Parser::ParseSet(TokenStream& toks) {  
  switch (length(toks)) {
  case 2: {
    auto name = toks.NextToken();
    auto expr = ParseToken(toks.NextToken());
    auto var = module.LocalSymbol(name);

    if (var) {
      var->ExtendWith(TypeDeducer::Run(expr));
      return new SetVar{name, expr};
    } else {
      module.UpdateGlobalSymbol(name, TypeDeducer::Run(expr));
      return new SetVar{name, expr};
    }
  }
  case 3: {
    auto obj_name = toks.NextToken();
    auto attr_name = toks.NextToken();
    auto expr = ParseToken(toks.NextToken());
    auto var = module.Symbol(obj_name);
    auto attr = module.Struct(var->Tag())->Attr(attr_name);

    return new SetAttr{obj_name, attr, expr};
  }
  default:
    throw "set! invalid number of arguments";
  }
}

Node* Parser::ParseDef(TokenStream& toks) {
  auto name = toks.NextToken();
  auto expr = ParseToken(toks.NextToken());

  return new DefVar{name, expr, module.DefineLocal(name, TypeDeducer::Run(expr))};
}

Node* Parser::ParseIf(TokenStream& toks) {
  auto cond = ParseToken(toks.NextToken());
  auto on_true = ParseToken(toks.NextToken());
  auto on_false = ParseToken(toks.NextToken());

  return new If{cond, on_true, on_false};
}

// (struct range 0 1 2)
// struct range _ = {0, 1, 2};
Node* Parser::ParseStruct(TokenStream& toks) {
  // 1) check if struct exist
  // 2) validate init values
  auto s = module.Struct(toks.NextToken());
  if (s) {
    std::vector<Node*> initializers;
    while (!toks.NextToken().IsEof()) {
      initializers.push_back(ParseToken(toks.CurrentToken()));
    }

    return new CompoundLiteral{std::move(initializers), s->type};
  } else {
    throw "type not exist";
  }
}

Node* Parser::ParseGet(TokenStream& toks) {
  dt::StrView obj_name = toks.NextToken();
  auto var = module.Symbol(obj_name);
  auto key = ParseToken(toks.NextToken());
  auto key_ty = TypeDeducer::Run(key);

  if (key_ty.IsSym()) {
    auto attr = module.Struct(var->Tag())->Attr(static_cast<Sym*>(key)->datum);
    return new AttrAccess{obj_name, attr};
  } else {
    throw "dynamic lookup is not supported yet";
  }
}

Node* Parser::ParseQuote(TokenStream& toks) {
  auto quotation = toks.NextToken();

  if (!toks.NextToken().IsEof()) {
    throw "can quote only 1 form";
  }

  switch (quotation.Tag()) {
  case Token::INT:
  case Token::REAL:
  case Token::STR:
    return ParseToken(quotation);
  case Token::WORD:
    return new ast::Sym{quotation};
  case Token::LIST:
    throw "no support for quoted lists yet";
  default:
    throw "quoting something unexpected";
  }
}

Node* Parser::ParseAttrAccess(TokenStream& toks) {
  dt::StrView obj_name = toks.NextToken();
  dt::StrView attr_name = toks.NextToken();

  auto var = module.Symbol(obj_name);
  auto attr = module.Struct(var->Tag())->Attr(attr_name);

  return new AttrAccess{obj_name, attr};
}

std::vector<ast::Node*> Parser::CollectParsed(TokenStream& toks) {
  std::vector<Node*> nodes;

  while (!toks.NextToken().IsEof()) {
    nodes.push_back(ParseToken(toks.CurrentToken()));
  }

  return nodes;
}
