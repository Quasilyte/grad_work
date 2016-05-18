#include <cstring>
#include <cstdio>
#include "lex/token.hpp"
#include "lex/token_stream.hpp"
#include "dbg/lex.hpp"
#include "dbg/dt.hpp"
#include "cc/parser.hpp"
#include "cc/code_gen.hpp"
#include "io/file_writer.hpp"
#include "sym/generator.hpp"
#include "sym/type.hpp"
#include "ast/node.hpp"
#include "cc/classifier.hpp"
#include "dt/dict.hpp"
#include "ast/atoms.hpp"
#include "ast/defs.hpp"
#include "backend/cpp/cg/code_writer.hpp"
#include "backend/cpp/cg/translator.hpp"
#include "ast/builtins.hpp"
#include <fstream>
#include <string>
#include "dt/alist.hpp"

std::string slurp(const char* path) {
  std::ifstream input{path};
  return std::string(
    std::istreambuf_iterator<char>{input},
    std::istreambuf_iterator<char>{}
  );
}

// quote can yield:
// * symbol
// * self-quoting value (int, real, etc.)
// * list of quoted values (symbols, ints, etc.)
// so, basically, the only "magical" value is symbol.
// also, quote of quote is, of course, results in list
// which car is "quote".

// int main(int argc, char* argv[]) {
int main() {
  using namespace lex;
  using namespace cc;
  using namespace cpp_cg;

  try {
    const char* input = R"lisp(
                        (#;
        (#struct range (int low high step))

        (#def (next (range r))
          (struct range (+ (. r low) (. r step))
                        (. r high)
                        (. r step)))

        (#def (current (range r))
          (. r step)))

        (#struct range (int low high step))

        (#def (next! (range r))
          (set! r low 3)
          r)
    )lisp";

    Translator::Run(Parser::Run(Classifier::Run(input)));
    // Parser::Run(Classifier::Run(input));
  } catch (const char* msg) {
    std::fprintf(stderr, "error: %s\n", msg);
  }

  /*
  try {
    std::string input = slurp(argv[1]);
    if (input.size()) {
      // const char* input = "(define x (if 1 1 0)) (set! x 10)";
      io::FileWriter fw{};
      cc::Parser parser{input.c_str()};
      cc::CodeGen cg{parser.Module(), parser.Parse()};
      cg.WriteTo(fw);
    } else {
      throw "file not found";
    }
  } catch (const char* msg) {
    std::fprintf(stderr, "error: %s\n", msg);
  }
  */

  return 0;
}
