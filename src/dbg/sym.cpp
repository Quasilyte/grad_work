#include <dbg/sym.hpp>

#include <deps/c/stdio.hpp>

using namespace sym;

void dbg::dump(sym::Type ty) {
  switch (ty.Tag()) {
  case Type::VOID: puts("<VOID>"); break;
  case Type::ANY: puts("<ANY>"); break;
  case Type::INT: puts("<INT>"); break;
  case Type::REAL: puts("<REAL>"); break;
  case Type::STR: puts("<STR>"); break;

  default:
    if (ty.IsStruct()) {
      puts("<STRUCT>");
    } else {
      puts("<UNKNOWN>");
    }
  }
}
