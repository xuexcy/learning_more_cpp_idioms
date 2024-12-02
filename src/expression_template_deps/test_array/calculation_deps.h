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
#include <cstddef>
#include <vector>

#include "expression_template_deps/array/concepts.h"
#include "expression_template_deps/array/operator.h"
#include "expression_template_deps/array/expression_template/expression.h"

static constexpr size_t kPrime_1 = 19;
static constexpr size_t kPrime_2 = 23;

template <size_t N>
using StdArray = std::array<double, N>;
using StdVector = std::vector<double>;

template <size_t N>
struct ArraySize {
    static const size_t kValue;
};  // struct ArraySize
template <size_t N>
const size_t ArraySize<N>::kValue= N;

template <ArrayLike A, ArrayLike B>
void init_array(A& a, B& b) {
    assert(a.size() == b.size());
    for (auto i = 0; i < a.size(); ++i) {
        a[i] = i % kPrime_1;
        b[i] = i % kPrime_2;
    }
}

const double EPSILON{0.000001};

// #define assert_double_eq(l, r, msg) \
//     if (EPSILON < std::abs(l - r)) { std::println("Assertion failed: ({} == {}).\n{} != {}, msg: {}, function {}, file {}, line {}", #l, #r, l, r, msg, __FUNCTION__, __FILE__, __LINE__); std::abort(); }

#define CALCULATE(a, b) \
    a + a * a + 3.4 + 0.2 + b * a + 1.1 * 1.2 + 1.2 * a * 0.3 * 1.2 * b +\
    b * a * a + a * b + 0.2 * b
namespace array_operator_overload {
template <ArrayLike T>
T calculate(const T& a, const T& b) {
    // operator + 和 operator * 在 array_operator.h 中重载
    return CALCULATE(a, b);
}
}  // namespace array_operator_overload

// 表达式求值
template <ArrayLike T>
T calculate_by_expression(const T& a, const T& b) {
    T res = get_array<T>(a.size());
    for (auto i = 0; i < a.size(); ++i) {
        res[i] = CALCULATE(a[i], b[i]);
    }
    // copy elision
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
inline auto calculate(const Array<A>& a, const Array<B>& b) {
    // auto = ExprType<Array<A>, Array<B>>
    return CALCULATE(a, b);
}

}  // namespace array_expression_template

