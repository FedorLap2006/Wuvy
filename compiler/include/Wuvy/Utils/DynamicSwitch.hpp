#include <map>
#include <vector>
#include <functional>
namespace Utils {
    template<typename _T>
    using CaseFunctionType =  std::function<void(_T)>;
    template<typename _T>
    using MutliCasesContainerType = std::map<std::vector<_T>,  CaseFunctionType<_T>>;
    template<typename _T>
    using CasesContainerType = std::map<_T,  CaseFunctionType<_T>>;

    template<typename _Value> 
    void dynamicSwitch(_Value v, MutliCasesContainerType<_Value> cases) {
        for(auto _case : cases) {
            for(auto caseValue : _case.first) {
                if (caseValue == v) _case.second(v);
                    
            }
        }
    }
    template<typename _Value, typename _DF> 
    void dynamicSwitch(_Value v, MutliCasesContainerType<_Value> cases, _DF const& dc) {
        bool c = false;
        for(auto _case : cases) {
            for(auto caseValue : _case.first) {
                if (caseValue == v) { _case.second(v); c=true; }
            }
        }
        if(!c) dc(v);
    }
    template<typename _Value> 
    void dynamicSwitch(_Value v, CasesContainerType<_Value> cases) {
        for(auto _case : cases) {
            for(auto caseValue : _case.first) {
                if (caseValue == v) _case.second(v);
                    
            }
        }
    }
    template<typename _Value, typename _DF> 
    void dynamicSwitch(_Value v, CasesContainerType<_Value> cases, _DF const& dc) {
        bool c = false;
        for(auto _case : cases) {
            for(auto caseValue : _case.first) {
                if (caseValue == v) { _case.second(v); c=true; }
            }
        }
        if(!c) dc(v);
    }

    template<typename _Value>
    struct DynamicCachedSwitch {
        // using CaseFunctionType = std::function<void(_Value)>;
        // using CasesContainerType = std::map<std::vector<_Value>, CaseFunctionType>;
        DynamicCachedSwitch(CasesContainerType<_Value> cases) {
            m_Cases = cases;
        }
        DynamicCachedSwitch(CasesContainerType<_Value> cases, CaseFunctionType<_Value> dc) {
            m_Cases = cases;
            m_DefaultCase = dc;
        }
        CasesContainerType<_Value> m_Cases;
        CaseFunctionType<_Value> m_DefaultCase = [](_Value v) -> void {};
        void do(_Value v) {
            DynamicSwitch(v, m_Cases, m_DefaultCase);
        }
    };
}

/*

DynamicCachedSwitch<int> sw({ {{10}, tenFunction} }, [](int v) -> void {});
sw.do(100);
dynamicSwitch(5, { {{5, 10}, fiveNtenFunction } });
dynamicSwitch(10, { {{5, 10}, fiveNtenFunction } });
dynamicSwitch(100, { {{5, 10}, fiveNtenFunction } }, [](int v) -> void {});
*/