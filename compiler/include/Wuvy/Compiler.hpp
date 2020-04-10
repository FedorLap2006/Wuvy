#pragma once

#include <Wuvy/Lexer.hpp>
// #include <Wuvy/Parser.hpp>
#include <Wuvy/Common/Types.hpp>
#include <string>


namespace Compiler {
  using namespace Common;
  namespace Language {
    extern Types::Dataset<std::string, int> keywordsSource;
    extern Types::Dataset<std::string, int> operatorsSource;
  }
  // Parser::AST parse(Lexer::Instance& lexer);
  void compile(std::string filepath, int generator);
}