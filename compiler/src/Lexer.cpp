#include <Wuvy/Lexer.hpp>
#include <Wuvy/Utils/DynamicSwitch.hpp>

#include <gtest/gtest.h>

#include <iostream>
#include <optional>
#include <sstream>
#include <cctype>
#include <unordered_map>
#include <functional>
#include <algorithm>

// #define COMPILER_LEXER_DEBUG_MODE

#ifdef COMPILER_LEXER_DEBUG_MODE
#define debug(...) std::cout << "[DEBUG] " << __VA_ARGS__ << std::endl; // TODO: remove all debug prints after complete
#else
#define debug(...) ;
#endif

namespace Compiler {
	namespace Lexer {
		namespace detail {

// NOTE: you need to define declare macro which will declare lexem and after using of this macros undefine it
#define OPERATORS_DECL_LIST		\
	declare(Add),	 		    \
	declare(Sub),	 		    \
	declare(Div),	 		    \
	declare(Mul),	 		    \
	declare(Pow),	 		    \
	declare(LogicNot),	  \
	declare(LogicAnd),	 	\
	declare(LogicOr),	 	  \
                        \
	declare(BinaryNot),	 	\
	declare(BinaryAnd),	 	\
	declare(BinaryOr),	 	\
	declare(PtrAccess),	 	\
	declare(PtrAddress),	\
                        \
	declare(LBracket),	 	\
	declare(RBracket),	 	\
	declare(LSBracket),	 	\
	declare(RSBracket),	 	\
	declare(LFBracket),	 	\
	declare(RFBracket),	 	\
	declare(LABracket),	 	\
	declare(RABracket),	 	\
                        \
	declare(Assign),      \
  declare(Equality),    \
                        \
  declare(Dot),         \
  declare(Comma),       \
  declare(Semicolon),   \
  declare(Colon),       \
  declare(NamespaceAccess), \
                            \
  declare(CompilerCall),    \
  declare(DecoratorUse)     \
  
// NOTE: you need to define declare macro which will declare lexem and after using of this macros undefine it
#define KEYWORDS_DECL_LIST 		\
	declare(If),	 		  \
	declare(Else),			\
	declare(Elif),	 		\
	declare(Func),	 		\
	declare(Var),	 	    \
  declare(Import)

			bool isNumber(const std::string& s) {
				std::string::const_iterator it = s.begin();
				while (it != s.end() && std::isdigit(*it)) ++it;
				return !s.empty() && it == s.end();
			}
			bool isHex(const std::string& s) {
				if(s[0] != '0' && s[1] != 'x' && s[1] != 'X') return false;
				
				std::string::const_iterator it = s.begin() + 2;
				while (it != s.end() && std::isxdigit(*it)) ++it;
				return !s.empty() && it == s.end();
			}
      bool isAll(const std::string& s, std::function<bool(const std::string&)> p) {
        std::string::const_iterator it = s.begin();
				while (it != s.end() && p(s)) ++it;
				return !s.empty() && it == s.end();
      }
      
			std::tuple<size_t, std::vector<std::shared_ptr<Lexems::KeywordBase>>> getMatchingKeywords( // get_keyword_count
          const std::string& s,
          std::function<bool(const std::string&, const std::string&)> matchFunc
          ) {
        static std::unordered_map<std::string, std::function<std::shared_ptr<Lexems::KeywordBase>(void)>> source({
#define declare(name) { Lexems::Keywords::name::source(), []() -> std::shared_ptr<Lexems::KeywordBase> { return std::make_shared<Lexems::Keywords::name>(); } }
          KEYWORDS_DECL_LIST
#undef declare
        });
        std::vector<std::shared_ptr<Lexems::KeywordBase>> targets{};
        // short int count = 0;
        for(auto iter : source) {
          if (matchFunc(s, iter.first)) {
            auto match = std::find_if(
              targets.begin(),
              targets.end(), [&iter](std::shared_ptr<Lexems::KeywordBase> kw) -> bool { return iter.first == kw->getSource(); });
            if(match == targets.end()) {
              targets.push_back(iter.second());
            } 
          }
        }
        return std::make_tuple(targets.size(), targets);
			}
			std::tuple<size_t, std::vector<std::shared_ptr<Lexems::OperatorBase>>> getMatchingOperators( // get_operator_count
          const std::string& s,
          std::function<bool(const std::string&, const std::string&)> matchFunc
          ) {
        static std::unordered_map<std::string, std::function<std::shared_ptr<Lexems::OperatorBase>(void)>> source({
#define declare(name) { Lexems::Operators::name::source(), []() -> std::shared_ptr<Lexems::OperatorBase> { return std::make_shared<Lexems::Operators::name>(); } }
          OPERATORS_DECL_LIST
#undef declare
        });
        std::vector<std::shared_ptr<Lexems::OperatorBase>> targets{};
        // short int count = 0;
        for(auto iter : source) {
          if (matchFunc(s, iter.first)) {
            auto match = std::find_if(
              targets.begin(),
              targets.end(), [&iter](std::shared_ptr<Lexems::OperatorBase> op) -> bool { return iter.first == op->getSource(); });
            if(match == targets.end()) {
              targets.push_back(iter.second());
            } 
          }
        }
        return std::make_tuple(targets.size(), targets);
			}
      
    
			std::tuple<bool, std::shared_ptr<Lexems::KeywordBase>> checkKeyword(const std::string& s) {
				std::tuple<bool, std::shared_ptr<Lexems::KeywordBase>> result;
				static Utils::DynamicSwitch<std::string, std::tuple<bool, std::shared_ptr<Lexems::KeywordBase>>&> kwSwitch({
#define declare(name) {{ Lexems::Keywords::name::source() }, [](std::string const& u, std::tuple<bool, std::shared_ptr<Lexems::KeywordBase>>& result) { result = std::make_tuple(true, std::make_shared<Lexems::Keywords::name>()); } }
					KEYWORDS_DECL_LIST
#undef declare
				}, [](std::string const& u, std::tuple<bool, std::shared_ptr<Lexems::KeywordBase>>& result) { result = std::make_tuple(false, std::make_shared<Lexems::Keywords::If>()); });
				kwSwitch(s, result);
				return result;
			}

			std::tuple<bool, std::shared_ptr<Lexems::OperatorBase>> checkOperator(const std::string& s) {
				std::tuple<bool, std::shared_ptr<Lexems::OperatorBase>> result;
				static Utils::DynamicSwitch<std::string, std::tuple<bool, std::shared_ptr<Lexems::OperatorBase>>&> opSwitch({
#define declare(name) {{ Lexems::Operators::name::source() }, [](std::string const&, std::tuple<bool, std::shared_ptr<Lexems::OperatorBase>>& result) { result = std::make_tuple(true, std::make_shared<Lexems::Operators::name>());  } }
					OPERATORS_DECL_LIST
#undef declare
				}, [](std::string const&, std::tuple<bool, std::shared_ptr<Lexems::OperatorBase>>& result) { result = std::make_tuple(false, nullptr); });
				opSwitch(s, result);
				return result;
			}
#undef KEYWORDS_DECL_LIST
#undef OPERATORS_DECL_LIST		
		}
		// std::optional<TokenValue> classifyToken(std::string token) {
		TokenValue classifyToken(std::string token) {
			// std::optional<TokenValue> result;
			TokenValue result;
			debug("token: " << token);
			{ 
				bool ok; std::shared_ptr<Lexems::KeywordBase> op;
				std::tie(ok, op) = detail::checkKeyword(token);
				if(ok) {
					debug("keyword appeared: " << op->getSource());
				       	result = op;
					goto ret;
				}
			}
			{
				bool ok; std::shared_ptr<Lexems::OperatorBase> op;

				std::tie(ok, op) = detail::checkOperator(token);
				if(ok) { 
					debug("operator appeared: " << op->getSource());
					result = op;
					goto ret;
				}
			}

			if (detail::isNumber(token)) {
				result = static_cast<unsigned long long>(std::stoull(token));
				goto ret;
			}
			if (detail::isHex(token)) {
				result = static_cast<unsigned long long>(std::stoull(token, nullptr, 16));
				goto ret;
			}

			result = token;
ret:;
			return result;


		}
		LexemsList lex(const std::string& source) {
			std::string cache;
			LexemsList lexems;
      			
			enum class State {
				None,
				Identifier,
				Digit,
				HexDigit,
				// OctalDigit,
				// BinaryDigit,
				Symbolic,
			}state = State::None;

			for(auto symbol : source) {
				switch (state) {
				case State::None:
					debug("state" << " None");
					if (std::isalpha(symbol)) {
						state = State::Identifier;
						cache = symbol;
						// continue;
					} else if (symbol == '0') { // TODO: delete it
						state = State::Digit;
						cache = symbol;
					} else if (std::isdigit(symbol)) {
						state = State::Digit;
						cache = symbol;
					} else if (std::isspace(symbol)) {
					} else if (std::ispunct(symbol)) {
						state = State::Symbolic;
						cache = symbol;
					}
					
					continue;
				case State::Identifier: // TODO: refractor of non-existing operators processing
					debug("state" << " Ident");
					if (std::isalpha(symbol)) cache += symbol;
					else if (std::isdigit(symbol)) {
						state = State::Digit;
						lexems.push_back(classifyToken(cache));
						cache = symbol;
					} else if (std::ispunct(symbol)) {
            auto opCountFunc = [&cache](const std::string& original) -> size_t { 
              return std::get<0>(
                detail::getMatchingOperators(original, [](const std::string& s, const std::string& e) -> bool { return e.rfind(s, 0) != std::string::npos; })
                ); };
            if(opCountFunc(std::string() + symbol) != 0) {
              state = State::Symbolic;
              lexems.push_back(classifyToken(cache));
              cache = symbol;
            } else {
              cache += symbol;
            }
					} else if (std::isspace(symbol)) {
						state = State::None;
						lexems.push_back(classifyToken(cache));
						cache = "";
					}
					continue;
				case State::HexDigit:
					debug("state" << " Hex");
					if (std::isxdigit(symbol)) cache += symbol;
					else if (std::isalpha(symbol)) {
						state = State::Identifier;
						lexems.push_back(classifyToken(cache));
						cache = symbol;
					} else if (std::ispunct(symbol)) {
						state = State::Symbolic;
						lexems.push_back(classifyToken(cache));
						cache = symbol;
					} else if (std::isspace(symbol)) {
						state = State::None;
						lexems.push_back(classifyToken(cache));
						cache = "";
					}
					continue;
				case State::Digit:
					debug("state" << " Digit");
					if (cache == "0") {
						if (symbol == 'x' || symbol == 'X') {
							state = State::HexDigit;
							cache += symbol;
						}
					} else if (std::isdigit(symbol)) {
						cache += symbol;
					} else if (std::isalpha(symbol)) {
						state = State::Identifier;
						lexems.push_back(classifyToken(cache));
						cache = symbol;
					} else if (std::ispunct(symbol)) {
						state = State::Symbolic;
						lexems.push_back(classifyToken(cache));
						cache = symbol;
					}
					continue;
				case State::Symbolic:
					debug("state" << " Symbolic");
					debug("> symbol => " << symbol);
					debug("> cache => " << cache);
					if (std::isalpha(symbol)) {
            auto opCountFunc = [&cache](const std::string& original) -> size_t { 
              return std::get<0>(
                detail::getMatchingOperators(original, [](const std::string& s, const std::string& e) -> bool { return e.rfind(s, 0) != std::string::npos; })
                ); };
            int count = opCountFunc(cache);
            if(!count) {
              if(opCountFunc(std::string() + symbol) != 0) {
                if (!cache.empty()) lexems.push_back(classifyToken(cache));
                cache = symbol;
              } else {
                cache += symbol;
              }
            } else {
              if (!cache.empty()) lexems.push_back(classifyToken(cache));
              state = State::Identifier;
              cache = symbol;
            }
					} else if (std::isdigit(symbol)) {
						state = State::Digit;
						if (!cache.empty()) lexems.push_back(classifyToken(cache));
						cache = symbol;
					} else if (std::isspace(symbol)) {
						state = State::None;
						lexems.push_back(classifyToken(cache));
						cache = "";
					} else if (std::ispunct(symbol)) {
            // int count = std::get<0>(detail::getMatchingOperators(cache, [](const std::string& s, const std::string& e) -> bool { return e.rfind(s, 0) != std::string::npos; }));
            // debug("> cache = " << cache << "| matches: " << count);
            // if(count == 1) {
              // if (!cache.empty()) lexems.push_back(classifyToken(cache));
              // cache = symbol;
            // } else {
              // cache += symbol;
            // }
            auto opCountFunc = [](const std::string& original) -> size_t { 
              return std::get<0>(
                detail::getMatchingOperators(original, [](const std::string& s, const std::string& e) -> bool { return e.rfind(s, 0) != std::string::npos; })
                ); };
            int count = opCountFunc(cache);
            if(count == 1) {
              if (!cache.empty()) lexems.push_back(classifyToken(cache));
              cache = symbol;
            } else if(count == 0) {
              if(opCountFunc(std::string() + symbol) != 0) {
                if (!cache.empty()) lexems.push_back(classifyToken(cache));
                cache = symbol;
              }
            } else {
              cache += symbol;
            }
						// if (!cache.empty()) lexems.push_back(classifyToken(cache));
					} else {
						std::cerr << "Lexer can't recognize symbol, please check it correctness: " << symbol;
					}
					continue;
				default:
					continue;
				}
			}
			if (!cache.empty()) lexems.push_back(classifyToken(cache));
			return lexems;
		}
		LexemsList lex(std::vector<std::string> sourceStrings) {
			std::string resultString;
			for(auto str : sourceStrings) { resultString = resultString + str + "\n"; }
			
			if (!resultString.empty()) resultString.pop_back();
			LexemsList lexems = lex(resultString);
			return lexems;
			// std::cerr << "lex(std::string source[]) is not implemented" << std::endl;
			// exit(1);
		}
		LexemsList lex(std::istream& stream) {
			std::stringstream ss;
			ss << stream.rdbuf();
			return lex(ss.str());
		}		
	}
}
// FIXME: move this structures into Utils
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::ostream& operator << (std::ostream& stream, const Compiler::Lexer::LexemsList& list) {
	using namespace Compiler::Lexer;
	for(auto lexem : list) {
		stream << "|";
		std::visit(overloaded {
				[&](std::shared_ptr<Lexems::KeywordBase> kw) {
					stream << " [keyword]: " << kw->getSource();
				},
				[&](std::shared_ptr<Lexems::OperatorBase> op) {
					stream << " [operator]: " << op->getSource();
				},
				[&](unsigned long long int num) {
					stream << " [number]: " << num;
				},
				[&](double floatNumber) {
					stream << " [fnumber]: " << floatNumber;
				},
				[&](const std::string& str) {
					stream << " [str]: " << str;
				},
				[&](auto lexem) {
					stream << " [unknown]: " << lexem;
				}
		}, lexem);
		stream << std::endl;
	}
	return stream;
}



