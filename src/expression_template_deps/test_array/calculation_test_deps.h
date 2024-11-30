/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/11/29 19:04:11
# Desc    :
########################################################################
*/
#pragma once


#include <array>
#include <vector>

#include "expression_template_deps/array/concepts.h"
#include "expression_template_deps/array/operator.h"
#include "expression_template_deps/array/expression_template/expression.h"

const size_t N = 1000;
const size_t PRIME_1 = 19;
const size_t PRIME_2 = 23;

using StdArray = std::array<double, N>;
using StdVector = std::vector<double>;

template <class T, class U>
void init_array(T& t, U& u) {
    for (auto i = 0; i < t.size(); ++i) {
        t[i] = i % PRIME_1;
        u[i] = i % PRIME_2;
    }
}

const double EPSILON{0.000001};

// #define assert_double_eq(l, r, msg) \
//     if (EPSILON < std::abs(l - r)) { std::println("Assertion failed: ({} == {}).\n{} != {}, msg: {}, function {}, file {}, line {}", #l, #r, l, r, msg, __FUNCTION__, __FILE__, __LINE__); std::abort(); }

#define CALCULATE(a, b) \
    a + a * a + 3.4 + 0.2 + b * a + 1.1 * 1.2 + 1.2 * a * 0.3 * 1.2 * b +\
    b * a * a + a * b + 0.2 * b
namespace array_operator_overload {
StdArray calculate(const StdArray& a, const StdArray& b) {
    // operator + 和 operator * 在 array_operator.h 中重载
    return CALCULATE(a, b);
}
}

// 表达式求值
StdArray calculate_by_expression(const StdArray& a, const StdArray& b) {
    StdArray res;
    for (auto i = 0; i < a.size(); ++i) {
        res[i] = CALCULATE(a[i], b[i]);
    }
    return res;
}

namespace array_expression_template {
template <class A, class B, class... Ts>
struct AddGetter {
    using type = AddGetter<Add<A, B>, Ts...>::type;
};  // AddGetter
template <class A, class B>
struct AddGetter<A, B> {
    using type = Add<A, B>;
};
template <class...Ts>
using ADD = AddGetter<Ts...>::type;

template <class A, class B, class... Ts>
struct MultiplyGetter {
    using type = MultiplyGetter<Multiply<A, B>, Ts...>::type;
};  // MultiplyGetter
template <class A, class B>
struct MultiplyGetter<A, B> {
    using type = Multiply<A, B>;
};
template <class...Ts>
using MULTIPLY = MultiplyGetter<Ts...>::type;

template <class A, class B>
using ExprType = ADD<
    A,  // a
    MULTIPLY<A, A>,  // a * a
    Variable<double>,  // 3.4
    Variable<double>,  // 0.2
    MULTIPLY<B, A>,  // b * a
    Variable<double>,  // 1.1 * 1.2
    MULTIPLY<Variable<double>, A, Variable<double>, Variable<double>, B>,  // 1.2 * a * 0.3 * 1.2 * b
    MULTIPLY<B, A, A>,  // b * a * a
    MULTIPLY<A, B>,  // a * b
    MULTIPLY<Variable<double>, B>  // 0.2 * b
>;
// 表达式模板求值
template <ArrayLike A, ArrayLike B>
inline auto calculate_by_expression_template(const Array<A>& a, const Array<B>& b) {
    // auto = ExprType<Array<A>, Array<B>>
    return CALCULATE(a, b);
}

}  // namespace array_expression_template

