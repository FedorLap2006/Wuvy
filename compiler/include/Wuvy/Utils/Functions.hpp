#include <Wuvy/Utils/Types.hpp>
#include <functional>

namespace Utils {
  namespace Functions {
    template<typename T, typename ...Args>  Types::PShared<T> pMakeShared(Args ...args) { return std::make_shared<T>(args...); }
    template<typename T, typename ...Args>  Types::PShared<T const> pConstMakeShared(Args ...args) { std::make_shared<T const>(args...); }  
    //bool isAllMatch(std::string string, std::function<bool (char)> pattern);
    bool isAllMatch(std::string string, std::function<int (int)> pattern);
  }
}