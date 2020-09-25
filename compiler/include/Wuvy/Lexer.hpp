#pragma once

#include <fstream>
#include <string>
#include <variant>
#include <vector>

namespace Compiler {
	namespace Lexer {
		namespace Lexems {
			// ======= Keywords =======
			
			/*
			 * \brief Base classs of all keywords lexems
			 */
			class KeywordBase {
			public:
				using Id = unsigned long;

				virtual std::string getSource() const = 0;
				virtual Id getId() const = 0;
			/*protected:
				Id m_Id;
				std::string m_Source;*/
			};
			
			namespace Keywords { // NOTE: when you probably will want to add/delete some of operators (not change!) you must to add/delete them from Lexer.cpp KEYWORDS_DECL_LIST

#define declare(Name, ID, Source) 					\
                                            \
class Name : public KeywordBase {					  \
public:									                    \
	Name() = default;						              \
	std::string getSource() const override {return Source;}		\
	Id getId() const override {return ID;}		            		\
	static auto id() -> Id;						                        \
	static auto source() -> std::string { return Source; }		\
}

				declare(If, 0, "if");
				declare(Else, 1, "else");
				declare(Elif, 2, "elif");
				declare(Func, 3, "fnc");
				declare(Var, 4, "var");
        declare(Import, 5, "import");
			}



			// ======= Operators =======

			/*
			 * \brief Base class of all operators lexems
			 */
			class OperatorBase {
			public:
				/*
				 * \brief Type of keyword ID in database
				 */
				using Id = unsigned long;

				virtual std::string getSource() const = 0;
				virtual Id getId() const = 0;

			};
			namespace Operators { // NOTE: when you probably will want to add/delete some of operators (not change!) you must to add/delete them from Lexer.cpp OPERATORS_DECL_LIST
#undef declare
#define declare(Name, ID, Source) 				\
								\
class Name : public OperatorBase {				\
public:								                    \
	Name() = default;					              \
	std::string getSource() const override {return Source;}	\
	Id getId() const override {return ID;}	            		\
                                                          \
	static auto id() -> Id;					                        \
	static auto source() -> std::string { return Source; }	\
}

				declare(Add, 1, "+");
				declare(Sub, 2, "-");
				declare(Div, 3, "/");
				declare(Mul, 4, "*");
				declare(Pow, 5, "**");
				declare(LogicNot, 6, "!");
				declare(LogicAnd, 7, "&&");
				declare(LogicOr, 8, "||");
				
				declare(BinaryNot, 9, "~");
				declare(BinaryAnd, 10, "&");
				declare(BinaryOr, 11, "|");

				declare(PtrAccess, 12, "*");
				declare(PtrAddress, 13, "&");
				
				declare(LBracket, 14, "(");
				declare(RBracket, 15, ")");
				declare(LSBracket, 16, "[");
				declare(RSBracket, 17, "]");
				declare(LFBracket, 18, "{");
				declare(RFBracket, 19, "}");
				declare(LABracket, 20, "<");
				declare(RABracket, 21, ">");
				
				declare(Assign, 22, "=");
        declare(Equality, 23, "==");
        
        declare(Dot, 24, ".");
        declare(Comma, 25, ",");
        declare(Semicolon, 26, ";");
        declare(Colon, 27, ":");
        declare(NamespaceAccess, 27, "::");
        declare(CompilerCall, 28, "@"); // SpecialCall
        declare(DecoratorUse, 28, "#");
#undef declare
				// Compiler::Lexer::Lexems::Operators::Sub
			}
		}

		using TokenValue = std::variant<std::shared_ptr<Lexems::KeywordBase>, std::shared_ptr<Lexems::OperatorBase>, unsigned long long int, double, std::string>;
		using LexemsList = std::vector<TokenValue>;

		LexemsList lex(std::istream& stream);
		LexemsList lex(const std::string& source);
		LexemsList lex(std::string source[]);

		// Utils functions
		// void printLexemsList(const LexemsList& list); // DEPRECATED
	}
}
std::ostream& operator << (std::ostream& stream, const Compiler::Lexer::LexemsList& list);
