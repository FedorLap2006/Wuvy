// #pragma once

// #include <exception>
// namespace Patterns {
//     class Visitable;
//     class BaseVisitor {
//     public:
//         class InvalidAcceptException : public std::exception {
//         public:
//             InvalidAcceptException(Visitable const& visited, BaseVisitor const& visitor) : visitor_(visitor), visited_(visited) {}
//             const char* what() const throw() {
//                 return "cannot accept visitor without implement of 'visit' method for the type";
//             }

//             BaseVisitor const& getVisitor() const { return this->visitor_; }
//             Visitable const& getVisited() const { return this->visited_; }

//         private:
//             BaseVisitor const& visitor_;
//             Visitable const& visited_;
//         };
//         virtual ~BaseVisitor() = default;
//         virtual void visit(Visitable const& visited) const {
//           throw InvalidAcceptException(visited, *this);
//         }
//     };

//     template <typename T>
//     class VisitorImpl {
//     public:
//         virtual void visit(T const&) const = 0;
//     };

//     template <typename... Types>
//     class Visitor : public BaseVisitor, public VisitorImpl<Types>... { };

//     class Visitable {
//     public:
//         virtual ~Visitable() = default;

//         virtual void accept(BaseVisitor const& visitor) const = 0;
        
//     protected:
//         template <typename T>
//         void acceptImpl(T const& visited, BaseVisitor const& visitor) const {
//             if(auto p = dynamic_cast<VisitorImpl<T> const*>(&visitor)) {
//                 p->visit(visited);
//             } else {
//                 visitor.visit(visited);
//             }
//         }
//     };
// #define MAKE_VISITABLE() \
//     virtual void accept(Patterns::BaseVisitor const& visitor) const override { return acceptImpl(*this, visitor); }
// }

#pragma once
 
#include <stdexcept>
#include <type_traits>
// #include <typeinfo>
// #include <functional>

namespace Patterns {
    template<typename>
    class Visitable;
 
    namespace Detail {
        template<typename R> 
        class BaseVisitor {
        public:
            explicit BaseVisitor() = default;
 
            virtual ~BaseVisitor() = default;
 
            BaseVisitor(BaseVisitor const &) = delete;
 
            BaseVisitor(BaseVisitor &&) = delete;
 
            BaseVisitor &operator=(BaseVisitor const &) = delete;
 
            BaseVisitor &operator=(BaseVisitor &&) = delete;
 
            virtual R visit(Visitable<R> &visited) {
                throw std::runtime_error("cannot accept visitor without implement of 'visit' method for the type");
            }
        };
 
        template<typename T, typename R = void>
        class VisitorImpl {
        public:
            virtual R visit(T &) = 0;
        };
    }
 
    template<typename R = void, typename... Types>
    class Visitor : public Detail::BaseVisitor<R>, public Detail::VisitorImpl<Types, R> ... {
    };
 
    template<typename R = void>
    class Visitable {
    public:
        explicit Visitable() = default;
 
        virtual ~Visitable() = default;
 
        Visitable(Visitable const &) = delete;
 
        Visitable(Visitable &&) = delete;
 
        Visitable &operator=(Visitable const &) = delete;
 
        Visitable &operator=(Visitable &&) = delete;
 
        virtual R accept(Detail::BaseVisitor<R> &visitor) = 0;
 
    protected:
        using ReturnType = R;
 
        template<typename T, typename PT>
        static R acceptImpl(T &visited, Detail::BaseVisitor<R> &visitor, PT const &predicate) {
            if (auto p = dynamic_cast<Patterns::Detail::VisitorImpl<T, R> *>(&visitor)) {
                return predicate(*p, visitor);
            } else {
                // std::string err("cannot accept visitor without implement of 'visit' method for the type: ");
                // err += std::string(typeid(visited).name());
                // throw std::runtime_error(err);
                visitor.visit(visited);
            }
        }
    };
}
 
#define MAKE_VISITABLE_CUSTOM(...) \
    virtual ReturnType accept(Patterns::Detail::BaseVisitor<ReturnType>& visitor) override \
    { return acceptImpl(*this, visitor, \
            [this](Patterns::Detail::VisitorImpl<std::decay_t<decltype(*this)>, ReturnType>& visitor, \
            Patterns::Detail::BaseVisitor<ReturnType>& baseVisitor) __VA_ARGS__ ); }
 
#define MAKE_VISITABLE() MAKE_VISITABLE_CUSTOM({ return visitor.visit(*this); })