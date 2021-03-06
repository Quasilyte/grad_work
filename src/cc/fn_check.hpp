#pragma once

#include <ast/typedefs.hpp>
#include <typedefs.hpp>

namespace sym {
  struct MonoFn;
}

namespace cc {
  void check_arity(sym::MonoFn*, uint arg_count);
  void check_arg_types(sym::MonoFn*, const ast::ArgList&s);
}
