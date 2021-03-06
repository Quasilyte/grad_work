#include <sym/fn.hpp>

using namespace sym;

NamedFn* MultiFn::Find(Key key) {
  auto it = funcs.find(key);

  if (it == funcs.end()) {
    return nullptr;
  } else {
    return it->second;
  }
}

Fn::Fn(Type type, ParamList&& params, ast::NodeList&& exprs, Type ret_type):
exprs{exprs}, params{params}, ret_type{ret_type}, type{type} {}

Fn::Fn(Type type, ParamList&& params, Type ret_type):
params{params}, ret_type{ret_type}, type{type} {}

Fn::Fn(ParamList&& params, Type ret_type):
params{params}, ret_type{ret_type} {}

uint Fn::Arity() const noexcept {
  return params.size();
}

const ParamList& Fn::Params() const noexcept {
  return params;
}

MonoFn::MonoFn(dt::StrView name, ParamList&& params, Type ret_type, Type type):
Fn{type, std::move(params), ret_type}, name{name} {}

void MonoFn::Define(ast::NodeList&& exprs) {
  this->exprs = exprs;
}

UnnamedFn::UnnamedFn(Type type, ParamList&& params, ast::NodeList&& exprs, Type ret_type):
Fn{type, std::move(params), std::move(exprs), ret_type} {}

NamedFn::NamedFn
(MultiFn* parent, ParamList&& params, Type ret_type, u32 suffix_idx):
Fn{std::move(params), ret_type}, parent{parent}, suffix_idx{suffix_idx} {}

void NamedFn::Define(ast::NodeList&& exprs, Type ty) {
  this->exprs = exprs;
  ret_type = ty;
}

MultiFn::MultiFn(Type type, dt::StrView name, uint arity):
name{name}, arity{arity}, type{type} {}
