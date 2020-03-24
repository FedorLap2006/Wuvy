#include <Wuvy/Utils/Functions.hpp>

namespace Utils {
  namespace Functions {
    // template<typename T, typename ...Args>  Types::PShared<T> PMakeShared(Args ...args) { return std::make_shared<T>(args...); }
    // template<typename T, typename ...Args>  Types::PShared<T const> PConstMakeShared(Args ...args) { std::make_shared<T const>(args...); }
    // bool isAllMatch(std::string s, std::function<bool (char)> pattern) { 
    //   std::string::const_iterator it = s.begin();
    //   while (it != s.end() && pattern(*it)) ++it;
    //   return !s.empty() && it == s.end(); 
    // }
    bool isAllMatch(std::string s, std::function<int (int)> pattern) {
      std::string::const_iterator it = s.begin();
      while (it != s.end() && pattern(*it)) ++it;
      return !s.empty() && it == s.end(); 
    }
  }
}