#include "lexer.h"
#include <cctype>
#include <cstring>
#include <iostream>
#include <utils.h>
#include <cstdlib>

namespace lexer {
	std::map<int, std::string> operators_database, keywords_database;
	void lex_file(std::string filepath) {
		std::ifstream file;
		file.open(filepath, file.in);
		int ln = 0;
		while(!file.eof()) {
			std::string s;
			std::getline(file, s);
			std::vector<lexer::Token> l = lex_string(filepath, ln++, s);
		}
	}

	lexer::Token identify_token(std::string file, int ln, int idx, std::string current_id) {
		lexer::Token t = {};
		for(auto op : lexer::operators_database) {
			if(op.second == current_id) {
				t.kind = lexer::Token::Kind::Operator;
				t.value.op = op.first;
				return t;
			}
		}
		for(auto kw : lexer::keywords_database) {
			if(kw.second == current_id) {
				t.kind = lexer::Token::Kind::Keyword;
				t.value.kw = kw.first;
				return t;
			}
		}
		if(isall_number(current_id)) {
			t.kind = lexer::Token::Kind::Number;
			t.value.num = std::atoi(current_id.c_str());
			return t;
		} else if(current_id.size() > 2, current_id[0] == '0' && (current_id[1] == 'x' || current_id[1] == 'X')) {
			//std::cout << current_id;
			t.kind = lexer::Token::Kind::Number;
			t.value.num = std::stoi(current_id, NULL, 16);
			return t;
		}
		if(isall_punct(current_id)) {
			errorf(lexer, "operator '" + current_id + "' is undefined", file, ln, idx);
			t.kind = lexer::Token::Kind::Error;
			return t;
		}
		if(current_id.size() > lexer::max_ident_len) {
			errorf(lexer, "identificator is too long", file, ln, idx);
			t.kind = lexer::Token::Kind::Error;
			return t;
		}
		t.kind = lexer::Token::Kind::Identificator;
		// std::cout << t.value.ident;
		strcpy(t.value.ident, current_id.c_str());
		return t;
	}

	std::vector<lexer::Token> lex_string(std::string file, int ln, std::string src) {
		std::string current_id;
		std::vector<lexer::Token> list;
		/*
		
		hello+world(aaa) tndjs

		hello
		+
		world
		(
		aaa
		)
		tndjs
	
		--------------------------
		h
		e
		l
		l
		o



		*/
		enum {
			state_digit,
			state_alpha,
			state_punct,
			state_wait,
		}state = state_wait;
		int cidx = 0;
		for(char ch : src) {
			// if(std::isalpha(ch)) {
			// 	current_id += ch;
			// 	std::cout << "isalpha: " << current_id;
			// }
			// else if(std::isdigit(ch) && !current_id.empty()) {
			// 	current_id += ch;
			// 	std::cout << "isdigit: " << current_id;
				
			// }
			// else if(std::isspace(ch)) {
			// 	std::cout << "isspace: " << current_id;
			// 	if(!current_id.empty()) {
			// 		list.push_back(identifyToken(file, ln, current_id));
			// 		current_id = "";
			// 	}
			// } else if(std::ispunct(ch)) {
			// 	std::cout << "ispunct: " << current_id;
			// 	if(current_id.empty()) {
			// 		current_id += ch;
			// 	}
			// 	else {
			// 		list.push_back(identifyToken(file, ln, current_id));
			// 		current_id = "" + ch;
			// 	}
			// }
			// switch (state) {
			// 	case state_alpha:
			// 		if(std::isalpha(ch) || std::isdigit(ch)) {
			// 			current_id += ch;
			// 		} else if(current_id.empty()) {

			// 		} else {
			// 			list.push_back(identifyToken(file, ln, current_id));
			// 			current_id = ;
			// 			break;
			// 		}
			// 	case state_digit:
			// 		if(std::isdigit(ch)) {

			// 		}
			// }
			// if(std::isalpha(ch) || (std::isdigit(ch) && state == state_alpha)) {
			// 	current_id += ch;
			// 	state = state_alpha;
			// } else if(std::isdigit(ch) && state == state_digit)

			if(std::isalpha(ch)) {
				switch(state) {
					case state_wait:
						state = state_alpha;
						current_id = "";
						current_id += ch;
						break;
					case state_alpha:
						current_id += ch;
						break;
					case state_digit:
						// if((current_id[0] == '0' && (ch == 'x' || ch == 'X')) || ((current_id[0] == '0' && (current_id[1] == 'x' || current_id[1] == 'X')) && std::isxdigit(ch))) {
						// 	current_id += ch;
						// }
						if((ch == 'x' || ch == 'X') && current_id.size() == 1 && current_id[0] == '0') {
							current_id += ch;
						} else if(current_id[0] == '0' && (current_id[1] == 'x' || current_id[1] == 'X') && std::isxdigit(ch)) {
							current_id += ch;
						} else if(current_id[0] == '0' && (current_id[1] == 'x' || current_id[1] == 'X') && !std::isxdigit(ch)) {
							lexer::Token r = identify_token(file, ln, cidx - current_id.size(), current_id);

							if(r.kind != lexer::Token::Kind::Error)
								list.push_back(r);
							state = state_alpha;
							current_id = "";
							current_id += ch;
						}
						// if(((current_id[0] == '0' && (current_id[1] == 'x' || current_id[1] == 'X')) && std::isxdigit(ch)) || (current_id[0] == '0' && (ch == 'x' || ch == 'X'))) {
						// 	current_id += ch;	
						// }
						break;
					default: {
						lexer::Token r = identify_token(file, ln, cidx - current_id.size(), current_id);

						if(r.kind != lexer::Token::Kind::Error)
							list.push_back(r);
						current_id = "";
						current_id += ch;
						state = state_alpha;
						break;
					}
				}
			} else if(std::isdigit(ch)) {
				switch(state) {
					case state_wait:
						state = state_digit;
						current_id = "";
						current_id += ch;
						break;
					case state_alpha: case state_digit:
						current_id += ch;
						break;
					default: {
						lexer::Token r = identify_token(file, ln, cidx - current_id.size(), current_id);

						if(r.kind != lexer::Token::Kind::Error)
							list.push_back(r);
						current_id = "";
						current_id += ch;
						state = state_digit;
						break;
					}
				}
			} else if(std::isspace(ch)) {
				lexer::Token r = identify_token(file, ln, cidx - current_id.size(), current_id);

				if(r.kind != lexer::Token::Kind::Error)
					list.push_back(r);
				current_id = "";
				state = state_wait;
			} else if(std::ispunct(ch)) {
				switch(state) {
					case state_wait:
						state = state_punct;
						current_id = "";
						current_id += ch;
						break;
					case state_punct:
						current_id += ch;
						break;
					default: {
						lexer::Token r = identify_token(file, ln, cidx - current_id.size(), current_id);

						if(r.kind != lexer::Token::Kind::Error)
							list.push_back(r);
						current_id = "";
						current_id += ch;
						state = state_punct;
						break;
					}
				}
			}
			cidx++;
		}
		if(!current_id.empty()) {
			lexer::Token r = identify_token(file, ln, cidx - current_id.size(), current_id);

			if(r.kind != lexer::Token::Kind::Error)
				list.push_back(r);
		}
		for(auto v : list) {
			view_token(v);
		}
		return list;
	}

	void view_token(lexer::Token t) {
		switch(t.kind) {
			case lexer::Token::Kind::Operator:
				std::cout << "[operator] ";
				std::cout << lexer::operators_database[t.value.op];
				std::cout << std::endl;
				break;
			case lexer::Token::Kind::Identificator:
				std::cout << "[identify] ";
				std::cout << std::string(t.value.ident);
				std::cout << std::endl;
				break;
			case lexer::Token::Kind::Keyword:
				std::cout << "[keyword] ";
				std::cout << lexer::keywords_database[t.value.kw];
				std::cout << std::endl;
				break;
			case lexer::Token::Kind::Number:
				std::cout << "[number] ";
				std::cout << t.value.num;
				std::cout << std::endl;
				break;
			default:
				break;
		}

	}
}

/*

test goto(aaaa)

*/