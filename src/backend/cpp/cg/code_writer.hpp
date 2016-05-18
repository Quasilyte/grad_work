#pragma once

#include "ast/visitor.hpp"

namespace io {
  class FileWriter;
}

namespace sym {
  class Module;
}

namespace cpp_cg {
  class CodeWriter;
}

class cpp_cg::CodeWriter: public ast::Visitor {
public:
  static void Run(ast::Node*, const sym::Module&, const io::FileWriter&);

  CodeWriter(const sym::Module&, const io::FileWriter&);

  void Visit(ast::Node*) override;
  void Visit(ast::Int*) override;
  void Visit(ast::Real*) override;
  void Visit(ast::Str*) override;
  void Visit(ast::Sym*) override;
  void Visit(ast::Sum*) override;
  void Visit(ast::SetVar*) override;
  void Visit(ast::DefVar*) override;
  void Visit(ast::If*) override;
  void Visit(ast::Var*) override;
  void Visit(ast::FuncCall*) override;
  void Visit(ast::CompoundLiteral*) override;
  void Visit(ast::AttrAccess*) override;

private:
  const sym::Module& module;
  const io::FileWriter& fw;
};