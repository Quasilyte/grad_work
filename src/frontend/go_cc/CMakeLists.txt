project(go_cc)

set(
  go_cc_src
  phase1/classifier.cpp
  common/reader.cpp
  common/module.cpp
  common/types.cpp
  parser.cpp
  # char_groups.cpp
  # reader.cpp
  expr_parsing.cpp
  stmt_parsing.cpp
  parsing.cpp
  fn_parsing.cpp
  # type_parsing.cpp
)

add_library(go_cc STATIC ${go_cc_src})
