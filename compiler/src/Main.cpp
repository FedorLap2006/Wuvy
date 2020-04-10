#include <Wuvy/Compiler.hpp>

int main(int argc, char const *argv[]) {
  Compiler::compile("compiler_test.wuvy", 0);
  return 0;
}