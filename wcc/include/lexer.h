#ifndef _LEXER_H_
#define _LEXER_H_

#include <string>
#include <map>
#include <fstream>
#include <vector>

namespace lexer {
	const int max_ident_len = 256;
	extern std::map<int, std::string> operators_database, keywords_database;

	struct Token {
		enum Kind {
			Operator,
			Identificator,
			Keyword,
			Number,
			Error,
		}kind;
		union Value {
			int op;
			int kw;
			int num;
			char ident[max_ident_len];
		}value;
	};


	void lex_file(std::string filepath);
	std::vector<lexer::Token> lex_string(std::string file, int ln, std::string src);
	void view_token(Token t);
}


#endif