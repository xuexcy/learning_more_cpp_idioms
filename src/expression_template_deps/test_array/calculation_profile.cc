/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/12/02 17:01:11
# Desc    :
########################################################################
*/

#include "calculation_deps.h"
#include "expression_template_deps/array/operator.h"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <print>

static constexpr uint16_t kLoopN{std::numeric_limits<uint16_t>::max()};
static constexpr uint16_t kArraySize{2000};
template <size_t N>
auto init_array() {
    StdArray<N> a;
    StdArray<N> b;
    init_array(a, b);
    return std::make_pair(a, b);
}


template <size_t N>
void calculate_by_operator_overload() {
    std::println("start calculate_by_operator_overload");
    auto [a, b] = init_array<N>();
    for (auto i = 0; i < kLoopN; ++i) {
        auto res= array_operator_overload::calculate(a, b);
        ++res[0];
    }
}

template <size_t N>
void calculate_by_expression() {
    std::println("start calculate_by_expression");
    auto [a, b] = init_array<N>();
    for (auto i = 0; i < kLoopN; ++i) {
        auto res = calculate_by_expression(a, b);
        ++res[0];
    }
}

template <size_t N>
void get_expression_by_et() {
    std::println("start get_expression_by_et");
    auto [data_a, data_b] = init_array<N>();
    using namespace array_expression_template;
    Array<decltype(data_a)> a(data_a);
    Array<decltype(data_b)> b(data_b);
    for (auto i = 0; i < kLoopN; ++i) {
        auto expr = calculate(a, b);
    }
}

template <size_t N>
void calculate_by_et() {
    std::println("start calculate_by_et");
    using namespace array_expression_template;
    auto [data_a, data_b] = init_array<N>();
    Array<decltype(data_a)> a(data_a);
    Array<decltype(data_b)> b(data_b);
    for (auto i = 0; i < kLoopN; ++i) {
        auto expr = calculate(a, b);
        decltype(data_a) res;
        collect_result(expr, &res);
        ++res[0];
    }
}

void run() {
    std::println("kLoopN: {}, kArraySize: {}", kLoopN, kArraySize);
    calculate_by_operator_overload<kArraySize>();
    calculate_by_expression<kArraySize>();
    get_expression_by_et<kArraySize>();
    calculate_by_et<kArraySize>();
}

int main() {
    run();
    return 0;
}
