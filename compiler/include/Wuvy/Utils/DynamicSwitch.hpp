#pragma once

#include <functional>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Utils {
    template <typename _Value, typename _Payload = void>
    struct DynamicSwitchTraits {
      using Value = _Value;
      using Payload = _Payload;
    };
    template <typename _Value>
    struct DynamicSwitchTraits<_Value, void> {
      using Value = _Value;
    };

    template<typename _Value, typename _Payload = void>
    class DynamicCase {
    public:
        DynamicCase() = default;
        virtual void operator()(_Value const& value, _Payload p) = 0;
    };

    template<typename _Value, typename _Payload>
    class DynamicCase<DynamicSwitchTraits<_Value, _Payload>> {
    public:
        DynamicCase() = default;
        virtual void operator()(_Value const& value, _Payload p) = 0;
    };

    template<typename _Value>
    class DynamicCase<_Value, void> {
    public:
        DynamicCase() = default;
        virtual void operator()(_Value const& value) = 0;
    };

    template<typename _Value>
    class DynamicCase<DynamicSwitchTraits<_Value, void>> {
    public:
        DynamicCase() = default;
        virtual void operator()(_Value const& value) = 0;
    };


    // template <typename _Value, typename _Payload = void>
    // class DynamicCase {
    //  public:
    // //   DynamicCase(std::function<void(_Value const&, _Payload p)> f) {
    // //     m_Function = [&, this](_Value const& v, _Payload p) -> void { f(v, p); };
    // //   }
    // //   DynamicCase(DynamicCase<_Value, _Payload>& ref) {
    // //     // m_Function = [&](_Value const& v, _Payload p) -> void { ref.operator()(v, p); };
    // //   }
    // //   DynamicCase() {
    // //     m_Function = [&, this](_Value const& v, _Payload p) -> void {
    // //     //  this->operator()(v, p);
    // //     };
    // //   }

    //   virtual void operator()(_Value const& value, _Payload p) = 0;
    //   /*
    //   {
    //     // m_Function(value, p);
    //   }
    //   */

    //  private:
    //   // bool self = false;
    //   // std::function<void(_Value const&, _Payload)> m_Function;
    // };

    // template <typename _Value>
    // class DynamicCase<_Value, void> {
    //  public:
    //   DynamicCase(std::function<void(_Value const&)> f) {
    //     m_Function = [&, this](_Value const& v) -> void { f(v); };
    //   }
    //   DynamicCase(DynamicCase<_Value>& ref) {
    //     m_Function = [&](_Value const& v) -> void { ref.operator()(v); };
    //   }
    //   DynamicCase() {
    //     // m_Function = [&, this](_Value const& v) -> void { this->operator()(v); };
    //   }

    //   virtual void operator()(_Value const& value) const { m_Function(value); }

    //  private:
    //   // bool self = false;
    //   std::function<void(_Value const&)> m_Function;
    // };

    // template <typename _Value, typename _Payload>
    // class DynamicCase<DynamicSwitchTraits<_Value, _Payload>> {
    //  public:
    //   DynamicCase(std::function<void(_Value const&, _Payload p)> f) {
    //     m_Function = [&, this](_Value const& v, _Payload p) -> void { f(v, p); };
    //   }
    //   DynamicCase(DynamicCase<_Value, _Payload>& ref) {
    //     m_Function = [&](_Value const& v, _Payload p) -> void { ref.operator()(v, p); };
    //   }
    // //   DynamicCase() {
    // //     m_Function = [&, this](_Value const& v, _Payload p) -> void {
    // //         // this->operator()(v, p);
    // //         auto vthis = dynamic_cast<T*>(this);
    // //         if (vthis) {
    // //             vthis->operator()(v, p);
    // //         }
    // //     };
    // //   }

    //   virtual void operator()(_Value const& value, _Payload p) const {
    //     m_Function(value, p);
    //   }

    //  private:
    //   // bool self = false;
    //   std::function<void(_Value const&, _Payload)> m_Function;
    // };

    // template <typename _Value>
    // class DynamicCase<DynamicSwitchTraits<_Value, void>> {
    //  public:
    //   DynamicCase(std::function<void(_Value const&)> f) {
    //     m_Function = [&, this](_Value const& v) -> void { f(v); };
    //   }
    //   DynamicCase(DynamicCase<_Value>& ref) {
    //     m_Function = [&](_Value const& v) -> void { ref.operator()(v); };
    //   }
    //   DynamicCase() {
    //     // m_Function = [&, this](_Value const& v) -> void { this->operator()(v); };
    //   }

    //   virtual void operator()(_Value const& value) const { m_Function(value); }

    //  private:
    //   // bool self = false;
    //   std::function<void(_Value const&)> m_Function;
    // };

    template <typename _Value, typename _Payload = void>
    class DynamicSwitch {
     public:
      using Traits = DynamicSwitchTraits<_Value, _Payload>;
      struct CaseWrapper {
        class LambdaCase : public DynamicCase<_Value, _Payload> {
        public:
            LambdaCase(std::function<void(_Value const&, _Payload)> _f) : m_Function(_f) {}
            LambdaCase(LambdaCase& other) : m_Function(other.m_Function) {}
            LambdaCase(std::shared_ptr<LambdaCase> other) : m_Function(other->m_Function) {}
            void operator() (_Value const& v, _Payload p) override {
                m_Function(v, p);
            }
        private:
            std::function<void(_Value const&, _Payload)> m_Function;
        };
        CaseWrapper(std::vector<_Value> _f,
                    std::function<void(_Value const&, _Payload)> _s)
            : first(_f), second(std::make_shared<LambdaCase>(_s)) {}
        CaseWrapper(std::vector<_Value> _f, std::shared_ptr<DynamicCase<_Value, _Payload>> _s)
            : first(_f), second(_s) {}

        std::vector<_Value> first;
        std::shared_ptr<DynamicCase<_Value, _Payload>> second;
      };
      DynamicSwitch(std::initializer_list<CaseWrapper> list,
                    std::function<void(_Value const&, _Payload)> def) {
        for (auto el : list) {
          for (_Value v : el.first) {
            if (!m_CaseList.count(v))
              // m_CaseList.insert(
              //     std::make_pair<
              //         _Value,
              //         std::shared_ptr<
              //             DynamicCase<_Value>
              //         >
              //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
              // );
              m_CaseList.emplace(
                  v, el.second);
          }
        }
        m_Default = def;
      }
      DynamicSwitch(std::initializer_list<CaseWrapper> list)
          : DynamicSwitch(list, [](_Value const& _unu1, _Payload _unu2) -> void {}) {}
      DynamicSwitch(std::function<void(_Value const&, _Payload)> def)
          : DynamicSwitch({}, def) {}
      DynamicSwitch() : DynamicSwitch({}) {}

      void operator()(_Value const& v, _Payload p) const {
        if (!m_CaseList.count(v))
          m_Default(v, p);
        else
          m_CaseList.at(v)->operator()(v, p);
      }
      void registerCase(CaseWrapper _case) {
        for (_Value v : _case.first) {
          if (!m_CaseList.count(v))
            // m_CaseList.emplace(
            //     v, std::make_shared<DynamicCase<_Value, _Payload>>(_case.second));
            m_CaseList.emplace(v, _case.second);
        }
      }
      std::shared_ptr<DynamicCase<_Value, _Payload>> getCase(_Value v) {
          if(!m_CaseList.count(v)) { return nullptr; }
          else return m_CaseList.at(v);
      }
      // void operator() (_Value const& v) const {
      //     if(!m_CaseList.count(v)) return;
      //     else m_CaseList.at(v)->operator()(v);
      // }
     private:
      std::unordered_map<_Value, std::shared_ptr<DynamicCase<_Value, _Payload>>>
          m_CaseList;
      std::function<void(_Value const&, _Payload)> m_Default;
    };
    template <typename _Value>
    class DynamicSwitch<_Value, void> {
     public:
      using Traits = DynamicSwitchTraits<_Value>;
      struct CaseWrapper {
        class LambdaCase : public DynamicCase<_Value> {
        public:
            LambdaCase(std::function<void(_Value const&)> _f) : m_Function(_f) {}
            LambdaCase(LambdaCase& other) : m_Function(other.m_Function) {}
            LambdaCase(std::shared_ptr<LambdaCase> other) : m_Function(other->m_Function) {}
            void operator() (_Value const& v) override {
                m_Function(v);
            }
        private:
            std::function<void(_Value const&)> m_Function;
        };
        CaseWrapper(std::vector<_Value> _f,
                    std::function<void(_Value const&)> _s)
            : first(_f), second(std::make_shared<LambdaCase>(_s)) {}
        CaseWrapper(std::vector<_Value> _f, std::shared_ptr<DynamicCase<_Value>> _s)
            : first(_f), second(_s) {}

        std::vector<_Value> first;
        std::shared_ptr<DynamicCase<_Value>> second;
      };
      DynamicSwitch(std::initializer_list<CaseWrapper> list,
                    std::function<void(_Value const&)> def) {
        for (auto el : list) {
          for (_Value v : el.first) {
            if (!m_CaseList.count(v))
              // m_CaseList.insert(
              //     std::make_pair<
              //         _Value,
              //         std::shared_ptr<
              //             DynamicCase<_Value>
              //         >
              //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
              // );
              m_CaseList.emplace(v,
                                 el.second);
          }
        }
        m_Default = def;
      }
      DynamicSwitch(std::initializer_list<CaseWrapper> list)
          : DynamicSwitch(list, [](_Value const& _) -> void {}) {}
      DynamicSwitch(std::function<void(_Value const&)> def) : DynamicSwitch({}, def) {}
      DynamicSwitch() : DynamicSwitch({}) {}

      void operator()(_Value const& v) const {
        if (!m_CaseList.count(v))
          m_Default(v);
        else
          m_CaseList.at(v)->operator()(v);
      }
      void registerCase(CaseWrapper _case) {
        for (_Value v : _case.first) {
          if (!m_CaseList.count(v))
            m_CaseList.emplace(v,
                               _case.second);
        }
      }
      std::shared_ptr<DynamicCase<_Value>> getCase(_Value v) {
          if(!m_CaseList.count(v)) { return nullptr; }
          else return m_CaseList.at(v);
      }
      // void operator() (_Value const& v) const {
      //     if(!m_CaseList.count(v)) return;
      //     else m_CaseList.at(v)->operator()(v);
      // }
     private:
      std::unordered_map<_Value, std::shared_ptr<DynamicCase<_Value>>> m_CaseList;
      std::function<void(_Value const&)> m_Default;
    };

    template <typename _Value, typename _Payload>
    class DynamicSwitch<DynamicSwitchTraits<_Value, _Payload>> {
     public:
      using Traits = DynamicSwitchTraits<_Value, _Payload>;
      struct CaseWrapper {
        class LambdaCase : public DynamicCase<Traits> {
        public:
            LambdaCase(std::function<void(_Value const&, _Payload)> _f) : m_Function(_f) {}
            LambdaCase(LambdaCase& other) : m_Function(other.m_Function) {}
            LambdaCase(std::shared_ptr<LambdaCase> other) : m_Function(other->m_Function) {}
            void operator() (_Value const& v, _Payload p) override {
                m_Function(v, p);
            }
        private:
            std::function<void(_Value const&, _Payload)> m_Function;
        };
        CaseWrapper(std::vector<_Value> _f,
                    std::function<void(_Value const&, _Payload)> _s)
            : first(_f), second(std::make_shared<LambdaCase>(_s)) {}
        CaseWrapper(std::vector<_Value> _f, std::shared_ptr<DynamicCase<Traits>> _s)
            : first(_f), second(_s) {}

        std::vector<_Value> first;
        std::shared_ptr<DynamicCase<Traits>> second;
      };
      DynamicSwitch(std::initializer_list<CaseWrapper> list,
                    std::function<void(_Value const&, _Payload)> def) {
        for (auto el : list) {
          for (_Value v : el.first) {
            if (!m_CaseList.count(v))
              // m_CaseList.insert(
              //     std::make_pair<
              //         _Value,
              //         std::shared_ptr<
              //             DynamicCase<_Value>
              //         >
              //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
              // );
              m_CaseList.emplace(v,
                                 el.second);
          }
        }
        m_Default = def;
      }
      DynamicSwitch(std::initializer_list<CaseWrapper> list)
          : DynamicSwitch(list, [](_Value const& _unu1, _Payload _unu2) -> void {}) {}
      DynamicSwitch(std::function<void(_Value const&, _Payload)> def)
          : DynamicSwitch({}, def) {}
      DynamicSwitch() : DynamicSwitch({}) {}

      void operator()(_Value const& v, _Payload p) const {
        if (!m_CaseList.count(v))
          m_Default(v, p);
        else
          m_CaseList.at(v)->operator()(v, p);
      }
      void registerCase(CaseWrapper _case) {
        for (_Value v : _case.first) {
          if (!m_CaseList.count(v))
            m_CaseList.emplace(v,
                               _case.second);
        }
      }
      std::shared_ptr<DynamicCase<Traits>> getCase(_Value v) {
          if(!m_CaseList.count(v)) { return nullptr; }
          else return m_CaseList.at(v);
      }
      // void operator() (_Value const& v) const {
      //     if(!m_CaseList.count(v)) return;
      //     else m_CaseList.at(v)->operator()(v);
      // }
     private:
      std::unordered_map<_Value, std::shared_ptr<DynamicCase<Traits>>> m_CaseList;
      std::function<void(_Value const&, _Payload)> m_Default;
    };

    template <typename _Value>
    class DynamicSwitch<DynamicSwitchTraits<_Value, void>> {
     public:
      using Traits = DynamicSwitchTraits<_Value, void>;
      struct CaseWrapper {
        class LambdaCase : public DynamicCase<Traits> {
        public:
            LambdaCase(std::function<void(_Value const&)> _f) : m_Function(_f) {}
            LambdaCase(LambdaCase& other) : m_Function(other.m_Function) {}
            LambdaCase(std::shared_ptr<LambdaCase> other) : m_Function(other->m_Function) {}
            void operator() (_Value const& v) override {
                m_Function(v);
            }
        private:
            std::function<void(_Value const&)> m_Function;
        };
        CaseWrapper(std::vector<_Value> _f,
                    std::function<void(_Value const&)> _s)
            : first(_f), second(std::make_shared<LambdaCase>(_s)) {}
        CaseWrapper(std::vector<_Value> _f, std::shared_ptr<DynamicCase<Traits>> _s)
            : first(_f), second(_s) {}

        std::vector<_Value> first;
        std::shared_ptr<DynamicCase<Traits>> second;
      };
      DynamicSwitch(std::initializer_list<CaseWrapper> list,
                    std::function<void(_Value const&)> def) {
        for (auto el : list) {
          for (_Value v : el.first) {
            if (!m_CaseList.count(v))
              // m_CaseList.insert(
              //     std::make_pair<
              //         _Value,
              //         std::shared_ptr<
              //             DynamicCase<_Value>
              //         >
              //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
              // );
              m_CaseList.emplace(v,
                                 el.second);
          }
        }
        m_Default = def;
      }
      DynamicSwitch(std::initializer_list<CaseWrapper> list)
          : DynamicSwitch(list, [](_Value const& _) -> void {}) {}
      DynamicSwitch(std::function<void(_Value const&)> def) : DynamicSwitch({}, def) {}
      DynamicSwitch() : DynamicSwitch({}) {}

      void operator()(_Value const& v) const {
        if (!m_CaseList.count(v))
          m_Default(v);
        else
          m_CaseList.at(v)->operator()(v);
      }

      void registerCase(CaseWrapper _case) {
        for (_Value v : _case.first) {
          if (!m_CaseList.count(v))
            m_CaseList.emplace(v,
                               _case.second);
        }
      }
      std::shared_ptr<DynamicCase<Traits>> getCase(_Value v) {
          if(!m_CaseList.count(v)) { return nullptr; }
          else return m_CaseList.at(v);
      }
      // void operator() (_Value const& v) const {
      //     if(!m_CaseList.count(v)) return;
      //     else m_CaseList.at(v)->operator()(v);
      // }
     private:
      std::unordered_map<_Value, std::shared_ptr<DynamicCase<Traits>>> m_CaseList;
      std::function<void(_Value const&)> m_Default;
    };

}

//#pragma once
//
//
//#include <functional>
//#include <initializer_list>
//#include <iostream>
//#include <memory>
//#include <type_traits>
//#include <unordered_map>
//#include <vector>
//
//
//// template<typename _T>
//// class AnyValueImpl {
//// public:
////     AnyValueImpl(_T v) : m_Value(v) {}
////     _T operator() {
////         return static_cast<_T>(m_Value);
////     }
//// private:
////     _T m_Value;
//// };
//
//// class AnyValue {
//// public:
////     template<typename _T>
////     AnyValue(_T v) {}
//
////     template<typename _T>
////     _T cast() const {
////         return m_ValueImpl;
////     }
//// private:
//
//// };
//
//namespace Utils {
//
//  template<typename _Value, typename _Payload = void>
//  struct DynamicSwitchTraits {
//      using Value = _Value;
//      using Payload = _Payload;
//  };
//  template<typename _Value>
//  struct DynamicSwitchTraits<_Value, void> {
//      using Value = _Value;
//  };
//
//  template<typename _Value, typename _Payload = void>
//  class DynamicCase {
//  public:
//      DynamicCase(std::function<void(_Value&, _Payload p)> f) {
//          m_Function = [&, this](_Value& v, _Payload p) -> void { f(v, p); };
//      }
//      DynamicCase(DynamicCase<_Value, _Payload> const& ref) {
//          m_Function = [&](_Value& v, _Payload p) -> void {
//              ref.operator()(v, p);
//          };
//      }
//
//      virtual void operator()(_Value& value, _Payload p) const {
//          m_Function(value, p);
//      }
//  private:
//      // bool self = false;
//      std::function<void(_Value&, _Payload)> m_Function;
//  };
//
//  template<typename _Value>
//  class DynamicCase<_Value, void> {
//  public:
//      DynamicCase(std::function<void(_Value&)> f) {
//          m_Function = [&, this](_Value& v) -> void { f(v); };
//      }
//      DynamicCase(DynamicCase<_Value> const& ref) {
//          m_Function = [&](_Value& v) -> void {
//              ref.operator()(v);
//          };
//      }
//
//      virtual void operator()(_Value& value) const {
//          m_Function(value);
//      }
//  private:
//      // bool self = false;
//      std::function<void(_Value&)> m_Function;
//  };
//
//  template<typename _Value, typename _Payload>
//  class DynamicCase<DynamicSwitchTraits<_Value, _Payload>> {
//  public:
//      DynamicCase(std::function<void(_Value&, _Payload p)> f) {
//          m_Function = [&, this](_Value& v, _Payload p) -> void { f(v, p); };
//      }
//      DynamicCase(DynamicCase<_Value, _Payload> const& ref) {
//          m_Function = [&](_Value& v, _Payload p) -> void {
//              ref.operator()(v, p);
//          };
//      }
//
//      virtual void operator()(_Value& value, _Payload p) const {
//          m_Function(value, p);
//      }
//  private:
//      // bool self = false;
//      std::function<void(_Value&, _Payload)> m_Function;
//  };
//
//  template<typename _Value>
//  class DynamicCase<DynamicSwitchTraits<_Value, void>> {
//  public:
//      DynamicCase(std::function<void(_Value&)> f) {
//          m_Function = [&, this](_Value& v) -> void { f(v); };
//      }
//      DynamicCase(DynamicCase<_Value> const& ref) {
//          m_Function = [&](_Value& v) -> void {
//              ref.operator()(v);
//          };
//      }
//
//      virtual void operator()(_Value& value) const {
//          m_Function(value);
//      }
//  private:
//      // bool self = false;
//      std::function<void(_Value&)> m_Function;
//  };
//
//
//  template<typename _Value, typename _Payload = void>
//  class DynamicSwitch {
//  public:
//      using Traits = DynamicSwitchTraits<_Value, _Payload>;
//      struct CaseWrapper {
//          CaseWrapper(std::vector<_Value> _f, DynamicCase<_Value, _Payload> const& _s) : first(_f), second(_s) {}
//
//          std::vector<_Value> first;
//          DynamicCase<_Value, _Payload> const& second;
//      };
//      DynamicSwitch(std::initializer_list<CaseWrapper> list, std::function<void(_Value&, _Payload)> def) {
//          for(auto el : list) {
//              for(_Value v : el.first) {
//                  if(!m_CaseList.count(v))
//                      // m_CaseList.insert(
//                      //     std::make_pair<
//                      //         _Value,
//                      //         std::shared_ptr<
//                      //             DynamicCase<_Value>
//                      //         >
//                      //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
//                      // );
//                      m_CaseList.emplace(v, std::make_shared<DynamicCase<_Value, _Payload>>(el.second));
//              }
//          }
//          m_Default = def;
//      }
//      DynamicSwitch(std::initializer_list<CaseWrapper> list) : DynamicSwitch(list, [](_Value& _unu1, _Payload _unu2) -> void {}) {}
//      DynamicSwitch() : DynamicSwitch({}) {}
//      DynamicSwitch(std::function<void(_Value&, _Payload)> def) : DynamicSwitch({}, def) {}
//
//      void operator() (_Value& v, _Payload p) const {
//          if(!m_CaseList.count(v)) return;
//          else m_CaseList.at(v)->operator()(v, p);
//      }
//      void registerCase(CaseWrapper _case) {
//          m_CaseList.emplace(std::make_pair<_Value, std::shared_ptr<DynamicCase<_Value, _Payload>>>(_case.first, _case.second));
//      }
//      std::function<void(_Value&, _Payload)> defaultCase() const { return m_Default; }
//      void defaultCase(std::function<void(_Value&, _Payload)> def) { m_Default = def; }
//
//      // void operator() (_Value& v) const {
//      //     if(!m_CaseList.count(v)) return;
//      //     else m_CaseList.at(v)->operator()(v);
//      // }
//  private:
//      std::unordered_map<_Value, std::shared_ptr<DynamicCase<_Value, _Payload>>> m_CaseList;
//      std::function<void(_Value&, _Payload)> m_Default;
//  };
//
//  template<typename _Value>
//  class DynamicSwitch<_Value, void> {
//  public:
//      using Traits = DynamicSwitchTraits<_Value>;
//      struct CaseWrapper {
//          CaseWrapper(std::vector<_Value> _f, DynamicCase<_Value> const& _s) : first(_f), second(_s) {}
//
//          std::vector<_Value> first;
//          DynamicCase<_Value> const& second;
//      };
//      DynamicSwitch(std::initializer_list<CaseWrapper> list, std::function<void(_Value&)> def) {
//          for(auto el : list) {
//              for(_Value v : el.first) {
//                  if(!m_CaseList.count(v))
//                      // m_CaseList.insert(
//                      //     std::make_pair<
//                      //         _Value,
//                      //         std::shared_ptr<
//                      //             DynamicCase<_Value>
//                      //         >
//                      //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
//                      // );
//                      m_CaseList.emplace(v, std::make_shared<DynamicCase<_Value>>(el.second));
//              }
//          }
//          m_Default = def;
//      }
//      DynamicSwitch(std::initializer_list<CaseWrapper> list) : DynamicSwitch(list, [](_Value& _) -> void {}) {}
//      DynamicSwitch() : DynamicSwitch({}) {}
//      DynamicSwitch(std::function<void(_Value&)> def) : DynamicSwitch({}, def) {}
//
//      void operator() (_Value& v) const {
//          if(!m_CaseList.count(v)) return;
//          else m_CaseList.at(v)->operator()(v);
//      }
//      void registerCase(CaseWrapper _case) {
//          m_CaseList.emplace(std::make_pair<_Value, std::shared_ptr<DynamicCase<_Value>>>(_case.first, _case.second));
//      }
//      std::function<void(_Value&)> defaultCase() const { return m_Default; }
//      void defaultCase(std::function<void(_Value&)> def) { m_Default = def; }
//      // void operator() (_Value& v) const {
//      //     if(!m_CaseList.count(v)) return;
//      //     else m_CaseList.at(v)->operator()(v);
//      // }
//  private:
//      std::unordered_map<_Value, std::shared_ptr<DynamicCase<_Value>>> m_CaseList;
//      std::function<void(_Value&)> m_Default;
//  };
//
//  template<typename _Value, typename _Payload>
//  class DynamicSwitch<DynamicSwitchTraits<_Value, _Payload>> {
//  public:
//      using Traits = DynamicSwitchTraits<_Value, _Payload>;
//      struct CaseWrapper {
//          CaseWrapper(std::vector<_Value> _f, DynamicCase<Traits> const& _s) : first(_f), second(_s) {}
//
//          std::vector<_Value> first;
//          DynamicCase<Traits> const& second;
//      };
//      DynamicSwitch(std::initializer_list<CaseWrapper> list, std::function<void(_Value&, _Payload)> def) {
//          for(auto el : list) {
//              for(_Value v : el.first) {
//                  if(!m_CaseList.count(v))
//                      // m_CaseList.insert(
//                      //     std::make_pair<
//                      //         _Value,
//                      //         std::shared_ptr<
//                      //             DynamicCase<_Value>
//                      //         >
//                      //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
//                      // );
//                      m_CaseList.emplace(v, std::make_shared<DynamicCase<Traits>>(el.second));
//              }
//          }
//          m_Default = def;
//      }
//      DynamicSwitch(std::initializer_list<CaseWrapper> list) : DynamicSwitch(list, [](_Value& _unu1, _Payload _unu2) -> void {}) {}
//      DynamicSwitch() : DynamicSwitch({}) {}
//      DynamicSwitch(std::function<void(_Value&, _Payload)> def) : DynamicSwitch({}, def) {}
//
//      void operator() (_Value& v, _Payload p) const {
//          if(!m_CaseList.count(v)) return;
//          else m_CaseList.at(v)->operator()(v, p);
//      }
//      void registerCase(CaseWrapper _case) {
//          m_CaseList.emplace(std::make_pair<_Value, std::shared_ptr<DynamicCase<Traits>>>(_case.first, _case.second));
//      }
//      std::function<void(_Value&, _Payload)> defaultCase() const { return m_Default; }
//      void defaultCase(std::function<void(_Value&, _Payload)> def) { m_Default = def; }
//      // void operator() (_Value& v) const {
//      //     if(!m_CaseList.count(v)) return;
//      //     else m_CaseList.at(v)->operator()(v);
//      // }
//  private:
//      std::unordered_map<_Value, std::shared_ptr<DynamicCase<Traits>>> m_CaseList;
//      std::function<void(_Value&, _Payload)> m_Default;
//  };
//
//
//  template<typename _Value>
//  class DynamicSwitch<DynamicSwitchTraits<_Value, void>> {
//  public:
//      using Traits = DynamicSwitchTraits<_Value, void>;
//      struct CaseWrapper {
//          CaseWrapper(std::vector<_Value> _f, DynamicCase<Traits> const& _s) : first(_f), second(_s) {}
//
//          std::vector<_Value> first;
//          DynamicCase<Traits> const& second;
//      };
//      DynamicSwitch(std::initializer_list<CaseWrapper> list, std::function<void(_Value&)> def) {
//          for(auto el : list) {
//              for(_Value v : el.first) {
//                  if(!m_CaseList.count(v))
//                      // m_CaseList.insert(
//                      //     std::make_pair<
//                      //         _Value,
//                      //         std::shared_ptr<
//                      //             DynamicCase<_Value>
//                      //         >
//                      //     >(v, std::make_shared<DynamicCase<_Value>>(el.second))
//                      // );
//                      m_CaseList.emplace(v, std::make_shared<DynamicCase<_Value>>(el.second));
//              }
//          }
//          m_Default = def;
//      }
//      DynamicSwitch(std::initializer_list<CaseWrapper> list) : DynamicSwitch(list, [](_Value& _) -> void {}) {}
//      DynamicSwitch() : DynamicSwitch({}) {}
//      DynamicSwitch(std::function<void(_Value&)> def) : DynamicSwitch({}, def) {}
//
//      void operator() (_Value& v) const {
//          if(!m_CaseList.count(v)) return;
//          else m_CaseList.at(v)->operator()(v);
//      }
//      void registerCase(CaseWrapper _case) {
//          m_CaseList.emplace(std::make_pair<_Value, std::shared_ptr<DynamicCase<Traits>>>(_case.first, _case.second));
//      }
//      std::function<void(_Value&)> defaultCase() const { return m_Default; }
//      void defaultCase(std::function<void(_Value&)> def) { m_Default = def; }
//      // void operator() (_Value& v) const {
//      //     if(!m_CaseList.count(v)) return;
//      //     else m_CaseList.at(v)->operator()(v);
//      // }
//  private:
//      std::unordered_map<_Value, std::shared_ptr<DynamicCase<Traits>>> m_CaseList;
//      std::function<void(_Value&)> m_Default;
//  };
//}
//
///*#include <unordered_map>
//#include <vector>
//#include <functional>
//#include <memory>
//#include <optional>
//
//namespace Utils {
//  using namespace Utils;
//  using namespace Common;
//
//  template <typename _ValueType> DynamicCaseFunctor = std::function <void(_ValueType&, std::optional <Common::Types::any>)>;
//
//  template <typename _ValueType>
//  class DynamicCase {
//  public:
//    DynamicCase() = default;
//    DynamicCase(DynamicCaseFunctor <_ValueType> f) {}
//    DynamicCase(DynamicCase const& other) {}
//
//    virtual void operator() (_ValueType& v, std::optional <Common::Types::any> p = {}) {
//      if(m_Functor) m_Functor->operator()(v, p);
//    }
//  private:
//    std::optional <DynamicSwitchCaseFunctor <_ValueType>> m_Functor;
//  };
//  template <typename _ValueType>
//  class DynamicSwitch {
//  public:
//    DynamicSwitch
//  };
//}*/
//
//
//
//// #include <map>
///*#pragma once
//
//#include <unordered_map>
//#include <vector>
//#include <functional>
//#include <memory>
//#include <any>
//#include <variant>
//#include <Wuvy/Common/Types.hpp>
//#include <Wuvy/Utils/Functions.hpp>
//#include <Wuvy/Utils/StdHelpers.hpp>
//#include <optional>
//namespace Utils {
//    using namespace Utils;
//    using namespace Common;
//
//
//    template<typename _ValueType>
//    using DynamicSwitchCaseFunctor = std::function <void(_ValueType&, std::optional <Common::Types::any>)>;
//
//    template<typename _ValueType>
//    class DynamicCase {
//    public:
//        DynamicCase() = default;
//        DynamicCase(DynamicSwitchCaseFunctor <_ValueType> f) : m_Function(f) {}
//        DynamicCase(DynamicCase const& other) {
//          if(other.m_Function) m_Function = other;
//
//        virtual void operator() (_ValueType& v, std::optional <Common::Types::any> p = {}) {
//            if(m_Function) (*m_Function)(v, p);
//        }
//    private:
//        std::optional <DynamicSwitchCaseFunctor <_ValueType>> m_Function;
//    };
//    template<typename _ValueType>
//    class DynamicSwitch {
//    public:
//        using CaseHandler = Types::PShared<DynamicCase<_ValueType>>;
//        DynamicSwitch(std::unordered_map<_ValueType, CaseHandler> cases) : m_DefaultCase(EmptyDefaultCaseHandler<_ValueType>()) {}
//        DynamicSwitch(std::unordered_map<std::vector<_ValueType>, CaseHandler> cases) : m_DefaultCase(EmptyDefaultCaseHandler<_ValueType>()) {}
//        DynamicSwitch(std::unordered_map<_ValueType, CaseHandler> cases, CaseHandler defcase) : m_DefaultCase(defcase) {}
//        DynamicSwitch(std::unordered_map<std::vector<_ValueType>, CaseHandler> cases, CaseHandler defcase) : m_DefaultCase(defcase) {
//        }
//
//        ~DynamicSwitch() = default;
//
///\*        template<typename _CaseType, typename... _ConstructorArgsTypes>
//        static CaseHandler makeCase(_CaseType c, _ConstructorArgsTypes... constructorArgs) {
//            return Functions::pMakeShared<_CaseType>(constructorArgs...);
//        }
//
//        void setCase(_ValueType v, Types::PShared<DynamicCaseBase> caseInst) {
//
//        }
//*\/
//        static CaseHandler makeCase(DynamicCase<_ValueType>& caseInstance) {
//          return Utils::pMakeShared<CaseHandler>(
//        }
//
//        void operator() (_ValueType v, Common::Types::any payload) {
//            std::visit(m_Cases, functorOverload {
//                [](std::unordered_map<_ValueType, CaseHandler>& cases) {
//                    bool c;
//                    for(auto _case : cases) {
//                        if(_case.first == v) { _case.second(v, payload); c = true; }
//                    }
//                    if(!c) m_DefaultCase(v, payload);
//                },
//                [](std::unordered_map<std::vector<_ValueType>, CaseHandler>& cases) {
//                    bool c;
//                    for(auto _case : cases) {
//                        for(auto _caseValue : _case.first) {
//                            if(_caseValue == v) { _case.second(v, payload); c = true; }
//                        }
//                    }
//                    if(!c) m_DefaultCase(v, payload);
//                },
//            });
//        }
//    private:
//        template<typename _ValueType>
//        class EmptyDefaultCaseHandler : public DynamicCaseBase<_ValueType> {
//            EmptyDefaultCaseHandler() = default;
//            void operator() (_ValueType v, Common::Types::any arguments) override {}
//        };
//        std::variant<std::unordered_map<_ValueType, CaseHandler>, std::unordered_map<std::vector<_ValueType>, CaseHandler>> m_Cases;
//        CaseHandler m_DefaultCase;
//    };
//    // template<typename _T>
//    // using CaseFunctionType =  std::function<void(_T)>;
//    // template<typename _T>
//    // using MultiCasesContainerType = std::map<std::vector<_T>,  CaseFunctionType<_T>>;
//    // template<typename _T>
//    // using CasesContainerType = std::map<_T,  CaseFunctionType<_T>>;
//    // // r<_Value, CaseFunctionType<_Value>>
//    // template<typename _Value>
//    // void dynamicSwitch(_Value v, MultiCasesContainerType<_Value> cases) {
//    //     for(auto _case : cases) {
//    //         for(auto caseValue : _case.first) {
//    //             if (caseValue == v) _case.second(v);
//
//    //         }
//    //     }
//    // }
//    // template<typename _Value, typename _DF>
//    // void dynamicSwitch(_Value v, MultiCasesContainerType<_Value> cases, _DF const& dc) {
//    //     bool c = false;
//    //     for(auto _case : cases) {
//    //         for(auto caseValue : _case.first) {
//    //             if (caseValue == v) { _case.second(v); c=true; }
//    //         }
//    //     }
//    //     if(!c) dc(v);
//    // }
//    // template<typename _Value>
//    // void dynamicSwitch(_Value v, MultiCasesContainerType<_Value> cases, Common::Types::any param) {
//    //     for(auto _case : cases) {
//    //         for(auto caseValue : _case.first) {
//    //             if (caseValue == v) _case.second(v, param);
//
//    //         }
//    //     }
//    // }
//    // template<typename _Value, typename _DF>
//    // void dynamicSwitch(_Value v, MultiCasesContainerType<_Value> cases, _DF const& dc) {
//    //     bool c = false;
//    //     for(auto _case : cases) {
//    //         for(auto caseValue : _case.first) {
//    //             if (caseValue == v) { _case.second(v); c=true; }
//    //         }
//    //     }
//    //     if(!c) dc(v);
//    // }
//    // template<typename _Value>
//    // void dynamicSwitch(_Value v, CasesContainerType<_Value> cases) {
//    //     for(auto _case : cases) {
//    //         if (_case.first == v) _case.second(v);
//    //     }
//    // }
//    // template<typename _Value, typename _DF>
//    // void dynamicSwitch(_Value v, CasesContainerType<_Value> cases, _DF const& dc) {
//    //     bool c = false;
//    //     for(auto _case : cases) {
//    //         if (_case.first == v) { _case.second(v); c=true; }
//    //     }
//    //     if(!c) dc(v);
//    // }
//
//    // template<typename _Value, bool mutli = false>
//    // struct DynamicCachedSwitch {
//    //     // using CaseFunctionType = std::function<void(_Value)>;
//    //     // using CasesContainerType = std::map<std::vector<_Value>, CaseFunctionType>;
//    //     DynamicCachedSwitch() {}
//    //     DynamicCachedSwitch(CasesContainerType<_Value> cases) {
//    //         m_Cases = cases;
//    //     }
//    //     DynamicCachedSwitch(CasesContainerType<_Value> cases, CaseFunctionType<_Value> dc) {
//    //         m_Cases = cases;
//    //         m_DefaultCase = dc;
//    //     }
//    //     void registerCase(_Value c, CaseFunctionType<_Value> f) {
//    //         // m_Cases.insert(std::make_pair<decltype(c), decltype(f)>(c, f));
//    //         m_Cases[c] = f;
//    //     }
//    //     template<typename _FT>
//    //     void setCase(_Value c, _FT const& f) {
//    //         // m_Cases.insert(std::make_pair(c, f));
//    //         m_Cases[c] = f;
//    //     }
//
//    //     void setCase(CaseFunctionType<_Value> f) {
//    //         m_DefaultCase = f;
//    //     }
//    //     template<typename _DT>
//    //     void setDefaultCase(_DT const& f) {
//    //         m_DefaultCase = f;
//    //     }
//
//    //     CasesContainerType<_Value> m_Cases;
//    //     CaseFunctionType<_Value> m_DefaultCase = [](_Value v) -> void {};
//    //     void select(_Value v) const {
//    //         dynamicSwitch(v, m_Cases, m_DefaultCase);
//    //     }
//    // };
//    // template<typename _Value>
//    // struct DynamicCachedSwitch<_Value, true> {
//    //     // using CaseFunctionType = std::function<void(_Value)>;
//    //     // using CasesContainerType = std::map<std::vector<_Value>, CaseFunctionType>;
//    //     DynamicCachedSwitch() {}
//    //     DynamicCachedSwitch(MultiCasesContainerType<_Value> cases) {
//    //         m_Cases = cases;
//    //     }
//    //     DynamicCachedSwitch(MultiCasesContainerType<_Value> cases, CaseFunctionType<_Value> dc) {
//    //         m_Cases = cases;
//    //         m_DefaultCase = dc;
//    //     }
//    //     void setCase(std::vector<_Value> c, CaseFunctionType<_Value> f) {
//    //         // m_Cases.insert(std::make_pair<decltype(c), decltype(f)>(c, f));
//    //         m_Cases[c] = f;
//    //     }
//    //     template<typename _FT>
//    //     void setCase(std::vector<_Value> c, _FT const& f) {
//    //         // m_Cases.insert(std::make_pair<decltype(c), decltype(f)>(c, f));
//    //         m_Cases[c] = f;
//    //     }
//    //     void setDefaultCase(CaseFunctionType<_Value> f) {
//    //         m_DefaultCase = f;
//    //     }
//    //     template<typename _DT>
//    //     void setDefaultCase(_DT const& f) {
//    //         m_DefaultCase = f;
//    //     }
//    //     MultiCasesContainerType<_Value> m_Cases;
//    //     CaseFunctionType<_Value> m_DefaultCase = [](_Value v) -> void {};
//    //     void select(_Value v) const {
//    //         dynamicSwitch(v, m_Cases, m_DefaultCase);
//    //     }
//    // };
//    // template<typename _Value>
//    // struct DynamicCachedSwitch<_Value, false> {
//    //     // using CaseFunctionType = std::function<void(_Value)>;
//    //     // using CasesContainerType = std::map<std::vector<_Value>, CaseFunctionType>;
//    //     DynamicCachedSwitch() {}
//    //     DynamicCachedSwitch(CasesContainerType<_Value> cases) {
//    //         m_Cases = cases;
//    //     }
//    //     DynamicCachedSwitch(CasesContainerType<_Value> cases, CaseFunctionType<_Value> dc) {
//    //         m_Cases = cases;
//    //         m_DefaultCase = dc;
//    //     }
//    //     void registerCase(_Value c, CaseFunctionType<_Value> f) {
//    //         m_Cases.insert(std::make_pair<decltype(c), CaseFunctionType<_Value>>(c, f));
//    //     }
//    //     template<typename _FT>
//    //     void registerCase(_Value c, _FT const& f) {
//    //         m_Cases.insert(std::make_pair<decltype(c), CaseFunctionType<_Value>>(c, f));
//    //     }
//    //     MultiCasesContainerType<_Value> m_Cases;
//    //     CaseFunctionType<_Value> m_DefaultCase = [](_Value v) -> void {};
//    //     void select(_Value v) const {
//    //         dynamicSwitch(v, m_Cases, m_DefaultCase);
//    //     }
//    // };
//}
//
//
// /*
//DynamicCachedSwitch<int> sw({ {{10}, tenFunction} }, [](int v) -> void {});
//sw.do(100);
//dynamicSwitch(5, { {{5, 10}, fiveNtenFunction } });
//dynamicSwitch(10, { {{5, 10}, fiveNtenFunction } });
//dynamicSwitch(100, { {{5, 10}, fiveNtenFunction } }, [](int v) -> void {});
//*/
//
