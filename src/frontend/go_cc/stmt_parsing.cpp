#include "frontend/go_cc/stmt_parsing.hpp"

#include "ast/builtins.hpp"
#include "ast/defs.hpp"
#include "ast/cond.hpp"
#include "ast/statements.hpp"
#include "frontend/go_cc/expr_parsing.hpp"
#include "frontend/go_cc/parsing.hpp"
#include "frontend/go_cc/char_groups.hpp"
#include "frontend/go_cc/cursor_ext.hpp"
#include "lex/cursor.hpp"
#include "unit/scope.hpp"
#include "cc/strict_arith.hpp"

using namespace lex;
#include <cstdio>
ast::Node* go_cc::parse_return(Cursor* cur) {
  skip(cur, SPACES);

  // auto ret = new ast::Return{parse_expr(next_expr(cur))};
  auto ret = new ast::Return{parse_expr(cur, ";\n")};
  skip(cur, 1);
  return ret;
}

ast::Node* go_cc::parse_short_var_decl(dt::StrView id, Cursor* cur) {
  skip(cur, SPACES);

  auto expr = parse_expr(cur, ";\n");
  skip(cur, 1);

  auto type = expr->Type();
  unit::scope_push(id, type);
  return new ast::DefVar{id, expr, type};
}

ast::Node* go_cc::parse_plus_assignment(dt::StrView id, Cursor* cur) {
  skip(cur, SPACES);
  return new ast::SumAssign{id, parse_expr(cur, ";\n")};
}

ast::Node* go_cc::parse_if(Cursor* cur) {
  std::vector<ast::Node*> on_true;
  std::vector<ast::Node*> on_false;

  auto cond = parse_expr(skip(cur, SPACES), "{");
  skip(cur, 1);

  // while (can_read(skip(&body_cur, SPACES))) {
  while (!try_consume(skip(cur, SPACES), '}')) {
    // Cursor cur{next_expr(&body_cur)};
    on_true.push_back(parse(cur));
  }

  if (try_consume(skip(cur, SPACES), "else")) {
    Cursor false_cur{read_group(skip(cur, SPACES), '{', '}').Truncate(1)};
    while (can_read(skip(&false_cur, SPACES))) {
      Cursor cur{next_expr(&false_cur)};
      on_false.push_back(parse(&cur));
    }
  }

  return cc::strict_if_stmt(cond, std::move(on_true), std::move(on_false));
}