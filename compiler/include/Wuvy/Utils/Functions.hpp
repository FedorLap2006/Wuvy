#include <Wuvy/Common/Types.hpp>
#include <functional>

namespace Utils {
  using namespace Common;
  namespace Functions {
    template<typename T, typename ...Args>  Types::PShared<T> pMakeShared(Args ...args) { return std::make_shared<T>(args...); }
    template<typename T, typename ...Args>  Types::PShared<T const> pConstMakeShared(Args ...args) { std::make_shared<T const>(args...); }  
    //bool isAllMatch(std::string string, std::function<bool (char)> pattern);
    bool isAllMatch(std::string string, std::function<int (int)> pattern);

    template<typename _To, typename _From>
    _To stcCast(_From v) { return static_cast<_To>(v); }
    template<typename _To, typename _From>
    _To dynCast(_From v) { return dynamic_cast<_To>(v); }
    template<typename _To, typename _From>
    _To reiCast(_From v) { return reinterpret_cast<_To>(v); }
  }
}