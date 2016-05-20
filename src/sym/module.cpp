#include "sym/module.hpp"

using namespace sym;

Module::Module(): name{dt::StrView{"global"}} {
  // Init builtins here
}

const dt::StrView& Module::Name() const noexcept {
  return name;
}

void Module::DefineStruct(dt::StrView name, std::vector<sym::Param>&& attrs) {
  if (type_name_map.Get(name)) {
    throw "redefinition of struct";
  } else {
    auto type_id = Type{type_name_map.Size()};
    auto s = new sym::Struct{name, std::move(attrs), sym::Type{type_id}};
    type_name_map.Put(name, s);
    type_id_map.push_back(s);
  }
}

sym::Struct* Module::Struct(dt::StrView name) const {
  return type_name_map.Get(name);
}

sym::Struct* Module::Struct(sym::Type::Id type_id) const {
  return type_id_map[type_id -  1];
}

void Module::DefineGlobalSymbol(dt::StrView name, Type ty) {
  if (globals.Get(name).IsVoid()) {
    globals.Put(name, ty);
  } else {
    throw "redefinition of global";
  }
}

Type Module::GlobalSymbol(dt::StrView name) const {
  return globals.Get(name);
}

void Module::DefineLocal(dt::StrView name, Type ty) {
  auto local = scope.LocalSymbol(name);

  if (local.IsVoid()) {
    scope.DefineSymbol(name, Type{ty});
  } else {
    throw "local already defined";
  }
}

Type Module::Symbol(dt::StrView name) {
  auto local = scope.Symbol(name);

  if (local.IsVoid()) {
    auto global = globals.Get(name);
    if (global.IsVoid()) {
      throw "unbound var referenced";
    } else {
      return global;
    }
  } else {
    return local;
  }
}

auto Module::Funcs() const noexcept -> dt::DictIter<MultiFunc*> {
  return funcs.Iter();
}

Type Module::LocalSymbol(dt::StrView name) {
  return scope.Symbol(name);
}

int Module::LocalsCount() const noexcept {
  return scope.LevelSize();
}

void Module::DeclareFunc(dt::StrView name, const sym::MultiFunc::Key& key, sym::Func* func) {
  auto multifunc = funcs.Get(name);

  if (multifunc) { // Has at least 1 definition
    // Has definition with same signature
    auto found_dup = multifunc->funcs.find(key) != multifunc->funcs.end();

    if (found_dup) {
      throw "func already defined";
    } else {
      multifunc->funcs[key] = func;
    }


  } else { // First declaration, no overloadings yet
    multifunc = new sym::MultiFunc{};
    multifunc->funcs[key] = func;

    funcs.Put(name, multifunc);
  }
}

MultiFunc* Module::Func(dt::StrView name) const {
  return funcs.Get(name);
}

void Module::CreateScopeLevel() {
  scope.CreateLevel();
}

void Module::DropScopeLevel() {
  scope.DropLevel();
}

