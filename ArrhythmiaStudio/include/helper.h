#pragma once

#include <sstream>

#define H_STR(x) #x
#define STRINGIFY(x) H_STR(x)

namespace std
{
    template <typename T>
    std::string to_string_with_precision(const T a_value, const int n = 6)
    {
        std::ostringstream out;
        out.precision(n);
        out << std::fixed << a_value;
        return out.str();
    }
}

template <class F>
struct lambda_traits : lambda_traits<decltype(&F::operator())>
{ };

template <typename F, typename R, typename... Args>
struct lambda_traits<R(F::*)(Args...)> : lambda_traits<R(F::*)(Args...) const>
{ };

template <class F, class R, class... Args>
struct lambda_traits<R(F::*)(Args...) const> {
    using pointer = typename std::add_pointer<R(Args...)>::type;

    static pointer cify(F&& f) {
        static F fn = std::forward<F>(f);
        return [](Args... args) {
            return fn(std::forward<Args>(args)...);
        };
    }
};

template <class F>
inline typename lambda_traits<F>::pointer cify(F&& f) {
    return lambda_traits<F>::cify(std::forward<F>(f));
}

struct pair_hash
{
    template <typename T, typename U>
    std::size_t operator()(const std::pair<T, U>& x) const
    {
        const size_t lhs = std::hash<T>()(x.first);
        const size_t rhs = std::hash<U>()(x.second);
        return lhs ^ (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
    }
};
