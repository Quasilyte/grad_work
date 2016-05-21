#pragma once

#include <cstdio>
#include "dt/str_view.hpp"

namespace io {
  class FileWriter;
}

namespace di {
  void set_files(FILE* module, FILE* runtime);
  void close_files();

  io::FileWriter module_writer();
  io::FileWriter runtime_writer();
}