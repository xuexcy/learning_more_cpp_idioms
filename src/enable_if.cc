/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/18 16:38:30
# Desc    :
########################################################################
通过 enable_if 来限制模板参数类型
1. enable_if 接受两个模板参数 <bool, class T = void>
2. 特化 bool = true，即 enable_if<true, T>
另外，在 C++20 可以 requires
*/

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <type_traits>

#include "cpp_utils/util.h"
#include "utils/main_decorator.h"

int negate(int i) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("negate({}) = {}", i, -i);
    return -i;
}
template <class F>
typename F::result_type negate(const F& f) {
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("template negate({}) = {}", f, -f);
    return -f;
}
// int::result_type negate(const int&);
struct String {
    std::string s;
    using result_type = decltype(s);
    const result_type operator-() const {
        std::string res = s;
        std::transform(s.begin(), s.end(), res.begin(), [](auto& ch) {
            return std::islower(ch) ? std::toupper(ch) : std::tolower(ch);
        });
        return res;
    }
};
template <>
struct std::formatter<String> : std::formatter<std::string> {
    auto format(const String& str, format_context& ctx) const {
        return formatter<string>::format(str.s, ctx);
    }
};

template <bool, class T = void>
struct my_enable_if {};
template <class T>
struct my_enable_if<true, T> {
    typedef T type;
};
template <class T>
typename my_enable_if<std::is_arithmetic<T>::value, T>::type square(T t) {
    // my_enable_if<true, T>::type -> T
    return t * t;
}
// 上下两种写法都可以
// 上面的写法不适用于构造函数和析构函数，因为他们没有返回类型
// 下面这种写法不适用于 operator，因为他们限制了参数个数，比如 operator+(T& lhs, T& rhs);
template <class T>
T square_v2(T t, typename my_enable_if<std::is_arithmetic<T>::value>::type* = nullptr) {
    // my_enable_if<true, void>::type -> T
    return t * t;
}

// c++20 requires
template <class T>
requires(std::is_arithmetic<T>::value)
T square_v3(T t) {
    return t * t;
}
const double EPSILON = 1e-9;
bool equal(double lhs, double rhs) {
    return std::abs(lhs - rhs) < EPSILON;
}
void run() {
    assert(-1 == negate(1));
    assert("AbC" == negate(String("aBc")));
    assert(9 == square(3));
    assert(9 == square_v2(3));
    assert(9 == square_v3(3));
    assert(equal(9.61, square(3.1)));
    assert(equal(9.61, square_v2(3.1)));
    assert(equal(9.61, square_v2(3.1)));
}
int main() {
    utils::MainDecorator::Access();
    run();
    return 0;
}

