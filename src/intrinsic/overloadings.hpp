#pragma once

#include "sym/type.hpp"

namespace intrinsic {
  sym::Type num_overloading(sym::Type x);
  sym::Type int_overloading(sym::Type x);
  sym::Type real_overloading(sym::Type x);
}
