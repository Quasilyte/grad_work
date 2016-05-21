#include "sym/lambda.hpp"

using namespace sym;

Lambda::Lambda(Type::Id type_id, ParamList&& params, Type ty):
params{params}, ret_type{ty}, type_id{type_id} {}

Lambda::Lambda(ParamList&& params, Type ty):
params{params}, ret_type{ty} {}

uint Lambda::Arity() const noexcept {
  return params.size();
}

const Lambda::ParamList& Lambda::Params() const noexcept {
  return params;
}

void Lambda::Define(ExprList &&exprs, Type ty) {
  ret_type = ty;
  this->exprs = exprs;
}
