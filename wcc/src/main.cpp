#include "compiler.h"
#include <fstream>

//using namespace lexer;

#define OpPlus 0x1
#define OpCall 0x2

#define KwGoto 0x1
#define KwPrint 0x2

int main() {
	lexer::operators_database = {
		//{ OpPlus, "+" },
		{ OpCall, "(" },
	};
	lexer::keywords_database = {
		{ KwGoto, "goto" },
		{ KwPrint, "print"},
	};
	lexer::lex_file("test.txt");
	return 0;
}