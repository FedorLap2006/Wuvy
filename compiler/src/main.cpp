#include <Wuvy/Lexer.hpp>
#include <iostream>
#include <gtest/gtest.h>
#include <fstream>

#define TESTS_ENABLED

// TODO: move tests running into separated environment e.g. TESTS_ENABLE flag

void processFile(std::string filepath) {
	std::fstream fs(filepath);
	std::cout << "Lexems list:" << std::endl << Compiler::Lexer::lex(fs);
}


int main(int argc, char **argv) {
#ifdef TESTS_ENABLED
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#else
	processFile("../examples/helloworld.wuvy");
	return 0;
#endif
}
