#include "backend/cpp/cg/utils.hpp"

#include "backend/cpp/cg/type_map.hpp"
#include "sym/module.hpp"
#include "sym/func.hpp"
#include "sym/lambda.hpp"
#include "sym/sym.hpp"
#include "di/output.hpp"
#include "io/file_writer.hpp"
#include "intrinsic/type_ops.hpp"
#include "cc/translation_unit.hpp"
#include "backend/cpp/cg/code_writer.hpp"

using namespace sym;
using namespace dt;
using namespace di;

Type deep_ret_type(Lambda* lambda, const cc::TranslationUnit& tu) {
  while (lambda->ret_type.IsFunc()) {
    lambda = lambda->ret_type.IsLambda()
        ? tu.lambdas[Type::LambdaKey(lambda->ret_type.Tag())]
        : tu.module.Func(lambda->ret_type.Tag());
  }

  return lambda->ret_type;
}

void cpp_cg::write_type(const cc::TranslationUnit& tu, Type ty) {
  if (ty.IsStruct()) {
    module_writer()("struct ")(tu.module.Struct(ty.Tag())->name);
  } else if (ty.IsFunc()) {
    // throw "func types are temporarily disabled";
    Lambda* lambda = ty.IsLambda()
        ? tu.lambdas[Type::LambdaKey(ty.Tag())]
        : tu.module.Func(ty.Tag());

    write_type(tu, deep_ret_type(lambda, tu));
  } else {
    module_writer()(type_name(ty));
  }
}

void cpp_cg::write_func_name(const Func* f) {
  module_writer()(f->name)(gen_suffix(f->suffix_idx));
}

void cpp_cg::write_lambda_name(const Lambda* l) {
  module_writer()('l')(gen_suffix(Type::LambdaKey(l->type_id)));
}

void cpp_cg::write_named_params(const cc::TranslationUnit& tu, const Lambda::ParamList& params) {
  if (params.size()) {
    module_writer()('(');
    for (uint i = 0; i < params.size() - 1; ++i) {
      write_type(tu, params[i].type);
      module_writer()(' ')(params[i].name)(',');
    }
    write_type(tu, params.back().type);
    module_writer()(' ')(params.back().name);
    module_writer()(')');
  } else {
    module_writer()("(void)");
  }
}

void cpp_cg::write_params(const cc::TranslationUnit& tu, const Lambda::ParamList& params) {
  if (params.size()) {
    module_writer()('(');
    for (uint i = 0; i < params.size() - 1; ++i) {
      write_type(tu, params[i].type);
      module_writer()(',');
    }
    write_type(tu, params.back().type);
    module_writer()(')');
  } else {
    module_writer()("(void)");
  }
}

void cpp_cg::write_intrinsic_params(Type ty) {
  auto arity = intrinsic::arity_of(ty);

  if (arity) {
    module_writer()('(');
    for (uint i = 0; i < arity - 1; ++i) {
      module_writer()(type_name(intrinsic::param_of(ty, i)))(',');
    }
    module_writer()(type_name(intrinsic::param_of(ty, arity - 1)))(')');
  } else {
    module_writer()("(void)");
  }
}
