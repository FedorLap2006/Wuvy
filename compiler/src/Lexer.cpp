#include <Wuvy/Lexer.hpp>
#include <fstream>
#include <iostream>


namespace Lexer {
  void Instance::run(std::string filepath) {
    std::ifstream file(filepath);
    std::string currentString;
    Location currentLocation;
    currentLocation.setFilepath(filepath);
    m_CurrentTokenList = {};
    while(!file.eof()) {
      std::getline(file, currentString);
      auto ret = this->processString(currentString, currentLocation);
      m_CurrentTokenList.insert(m_CurrentTokenList.end(), ret.begin(), ret.end());
      currentLocation.setLine(currentLocation.getLine() + 1);
    }
  }

  TokenList Instance::processString(std::string line, Location initialLoc) {
    TokenList resultList;
    enum {
      STATE_WAIT,
      STATE_CHAR,
      STATE_DIGIT,
      STATE_PUNCT,
    } state = STATE_WAIT;
    std::string currentCache;
    Location currentLoc = initialLoc;

    auto identifyToken = [&, this](std::string cache) {
      // std::cout << "state: " << state << " cache: " << currentCache;
      Location cachedLoc = currentLoc;
      currentLoc.setCol(currentLoc.getCol() + cache.size());
      for(auto iterator : this->m_Keywords) {
        if(iterator.second == cache) {
          resultList.push_back(Functions::pMakeShared<KeywordToken>(iterator.first, cachedLoc));
          // std::cout << " " << "keyword" << std::endl;
          return;
        }
      }
      for(auto iterator : this->m_Operators) {
        if(iterator.second == cache) {
          resultList.push_back(Functions::pMakeShared<OperatorToken>(iterator.first, cachedLoc));
          // std::cout << " " << "operator" << std::endl;
          return;
        }
      }

      if(Functions::isAllMatch(cache, [](int ch) -> int { return std::isdigit(ch); })) {
        resultList.push_back(Functions::pMakeShared<NumberToken>(std::atoi(cache.c_str()), cachedLoc));
        // std::cout << " " << "digit" << std::endl;
        return;
      } 

      if((cache.compare(0, 2, "0x") == 0 || cache.compare(0, 2, "0X") == 0) && Functions::isAllMatch(cache.substr(2), [](int ch) -> int { return std::isxdigit(ch); })) {
        resultList.push_back(Functions::pMakeShared<NumberToken>(std::stoi(cache, NULL, 16), cachedLoc));
        // std::cout << " " << "digit" << std::endl;
        return;
      }
      resultList.push_back(Functions::pMakeShared<IdentToken>(cache, cachedLoc));
      // std::cout << " " << "ident" << std::endl;
      return;
    };
//    // std::cout << line << std::endl;
    for(char currentChar : line) {
      //// std::cout << "state: " << state << " cache: " << currentCache << " char: " << currentChar << std::endl;
      switch(state) {
        case STATE_WAIT: {

          if(std::isalpha(currentChar)) {
            state = STATE_CHAR;
            currentCache = currentChar;
          } else if(std::isdigit(currentChar)) {
            state = STATE_DIGIT;
            currentCache = currentChar;
          } else if(std::ispunct(currentChar)) {
            state = STATE_PUNCT;
            currentCache = currentChar;
          } else if(std::isspace(currentChar)) {
            continue;
          }

          break;
        }
        case STATE_CHAR: {

          if(std::isalpha(currentChar)) {
            currentCache += currentChar;
          } else if(std::isdigit(currentChar)) {
            currentCache += currentChar;
          } else if(std::ispunct(currentChar)) {
            identifyToken(currentCache);
            state = STATE_PUNCT;
            currentCache = currentChar;
          } else if(std::isspace(currentChar)) {
            identifyToken(currentCache);
            state = STATE_WAIT;
            currentCache = "";
          }

          break;
        }
        case STATE_DIGIT: {

          if(std::isalpha(currentChar)) {
            if(currentCache.compare(0, 1, "0") && currentChar == 'x') {
              currentCache += currentChar;
            } else if((currentCache.compare(0, 2, "0x") || currentCache.compare(0, 2, "0X")) && std::isxdigit(currentChar)) {
              currentCache += currentChar;
            } else {
              identifyToken(currentCache);
              state = STATE_CHAR;
              currentCache = currentChar;
            }
          } else if(std::isdigit(currentChar)) {
            currentCache += currentChar;
          } else if(std::ispunct(currentChar)) {
            identifyToken(currentCache);
            state = STATE_DIGIT;
            currentCache = currentChar;
          } else if(std::isspace(currentChar)) {
            identifyToken(currentCache);
            state = STATE_WAIT;
            currentCache = "";
          }

          break;
        }
        case STATE_PUNCT: {

          if(std::isalpha(currentChar)) {
            identifyToken(currentCache);
            state = STATE_CHAR;
            currentCache = currentChar;
          } else if(std::isdigit(currentChar)) {
            identifyToken(currentCache);
            state = STATE_DIGIT;
            currentCache = currentChar;
          } else if(std::ispunct(currentChar)) {
            currentCache += currentChar;
          } else if(std::isspace(currentChar)) {
            identifyToken(currentCache);
            state = STATE_WAIT;
            currentCache = "";
          }

          break;
        }
        default: // never be used
          continue;
      }
    }
    if(currentCache != "") { identifyToken(currentCache); }
    return resultList;
  }
}
