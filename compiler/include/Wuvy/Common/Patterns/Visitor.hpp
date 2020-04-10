#pragma once

#include <stdexcept>
#include <type_traits>
namespace Common {
    namespace Patterns {
        namespace Detail {
            template<typename R>
            class BasicVisitor {
            public:
                explicit BasicVisitor() = default;

                virtual ~BasicVisitor() = default;

                BasicVisitor(BasicVisitor const &) = delete;

                BasicVisitor(BasicVisitor &&) = delete;

                BasicVisitor &operator=(BasicVisitor const &) = delete;

                BasicVisitor &operator=(BasicVisitor &&) = delete;

                virtual R catchInvalid() {
                    throw std::runtime_error(
                            "cannot accept visitor without implement of 'visit' method for the type");
                }
            };

            template<typename T, typename R = void>
            class VisitorImpl {
            public:
                virtual ~VisitorImpl() = default;

                virtual R visit(T &) = 0;
            };
        }

        template<typename TraitsT, typename... Types>
        class BaseVisitor :
                public Detail::BasicVisitor<typename TraitsT::ReturnType>,
                public Detail::VisitorImpl<Types, typename TraitsT::ReturnType> ... {
        public:
        //    using ReturnType = R;
        };

        template<typename TraitsT>
        class BaseVisitable {
        public:
            using Traits = TraitsT;
            using ReturnType = typename TraitsT::ReturnType;
            using CatchType = typename TraitsT::CatchType;

            explicit BaseVisitable() = default;

            virtual ~BaseVisitable() = default;

            BaseVisitable(BaseVisitable const &) = delete;

            // BaseVisitable(BaseVisitable &&) = delete;

            BaseVisitable &operator=(BaseVisitable const &) = delete;

            BaseVisitable &operator=(BaseVisitable &&) = delete;

            virtual ReturnType accept(Detail::BasicVisitor<ReturnType> &visitor) = 0;

        protected:
            template<typename T, typename PT>
            static ReturnType
            acceptImpl(T &visited, Detail::BasicVisitor<ReturnType> &visitor, PT const &predicate) {
                if (auto p = dynamic_cast<Patterns::Detail::VisitorImpl<T, ReturnType> *>(&visitor)) {
                    return predicate(p, visitor);
                }
                if (auto p =
                        dynamic_cast<Patterns::Detail::VisitorImpl<CatchType, ReturnType> *>(&visitor)) {
                    return predicate(p, visitor);
                }
                return visitor.catchInvalid();
            }
        };

        template<typename R = void, typename C = void>
        struct VisitorTraits {
            using ReturnType = R;
            using CatchType = C;
        };
        template<typename R>
        struct VisitorTraits<R, void> {
            using ReturnType = R;
            using CatchType = BaseVisitable<VisitorTraits<R>>;
        };


        template<typename... Types> using Visitor = BaseVisitor<VisitorTraits<>, Types...>;
        template<typename R = void, typename C = void> using Visitable = BaseVisitable<VisitorTraits<R, C>>;
    }
}
#define MAKE_VISITABLE_CUSTOM(...) \
    virtual ReturnType accept(Patterns::Detail::BasicVisitor<ReturnType>& visitorRef) override \
    { return acceptImpl(*this, visitorRef, [this](auto* visitor, auto& visitorBase) __VA_ARGS__ ); }

#define MAKE_VISITABLE() \
    MAKE_VISITABLE_CUSTOM({ return visitor->visit(*this); })
