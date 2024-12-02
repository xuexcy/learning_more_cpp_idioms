/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/11/29 12:44:21
# Desc    :
########################################################################
*/

#include "benchmark/benchmark.h"

#include "calculation_deps.h"
#include "expression_template_deps/array/operator.h"

template <size_t N, ArrayLike A, ArrayLike B=A>
auto init_array() {
    A a = get_array<A>(N);
    B b = get_array<B>(N);
    init_array(a, b);
    return std::make_pair(a, b);
}


template <size_t N, ArrayLike A, ArrayLike B=A>
static void BM_calculate_by_operator_overload(::benchmark::State& state) {
    auto [a, b] = init_array<N, A, B>();
    for (auto _ : state) {
        auto res= array_operator_overload::calculate(a, b);
        ++res[0];
    }
}

template <size_t N, ArrayLike A, ArrayLike B=A>
static void BM_calculate_by_expression(::benchmark::State& state) {
    auto [a, b] = init_array<N, A, B>();
    for (auto _ : state) {
        auto res = calculate_by_expression(a, b);
        ++res[0];
    }
}

template <size_t N, ArrayLike A, ArrayLike B=A>
static void BM_get_expression_by_et(::benchmark::State& state) {
    using namespace array_expression_template;
    auto [data_a, data_b] = init_array<N, A, B>();
    Array<A> a(data_a);
    Array<B> b(data_b);
    for (auto _ : state) {
        auto expr = calculate(a, b);
    }
}

template <size_t N, ArrayLike A, ArrayLike B, ArrayLike Res>
static void BM_calculate_by_et(::benchmark::State& state) {
    using namespace array_expression_template;
    auto [data_a, data_b] = init_array<N, A, B>();
    Array<A> a(data_a);
    Array<B> b(data_b);
    for (auto _ : state) {
        auto expr = calculate(a, b);
        Res res = get_array<Res>(N);
        collect_result(expr, &res);
        ++res[0];
    }
}

template <size_t N>
void RegisterBenchmark() {
    // array by operator overload
    benchmark::RegisterBenchmark(
        std::format("BM_calculate_by_operator_overload array size: {}", N),
        BM_calculate_by_operator_overload<N, StdArray<N>>);
    // vector by operator overload
    benchmark::RegisterBenchmark(
        std::format("BM_calculate_by_operator_overload vector size: {}", N),
        BM_calculate_by_operator_overload<N, StdVector>);

    // array by expression
    benchmark::RegisterBenchmark(
        std::format("BM_calculate_by_expression array size: {}", N),
        BM_calculate_by_expression<N, StdArray<N>>);
    // vector by expression
    benchmark::RegisterBenchmark(
        std::format("BM_calculate_by_expression vector size: {}", N),
         BM_calculate_by_expression<N, StdVector>);

    // et: expression template
    #define Register_get_expression_by_et(AType, a_name, BType, b_name) \
        benchmark::RegisterBenchmark( \
            std::format("BM_get_expression_by_et {}&{} size: {}", #a_name, #b_name, N), \
            BM_get_expression_by_et<N, AType, BType>);
    // get_expression_by_et
    // array&array
    Register_get_expression_by_et(StdArray<N>, array, StdArray<N>, array)
    // vector&vector
    Register_get_expression_by_et(StdVector, vector, StdVector, vector)
    // array&vector
    Register_get_expression_by_et(StdArray<N>, array, StdVector, vector)
    // vector&array
    Register_get_expression_by_et(StdVector, vector, StdArray<N>, array)

    #define Register_calculate_by_et(AType, a_name, BType, b_name, ResType, res_name) \
        benchmark::RegisterBenchmark( \
            std::format("BM_calculate_by_et res={} {}&{} size: {}", #res_name, #a_name, #b_name, N), \
            BM_calculate_by_et<N, AType, BType, ResType>);
    // array&array -> array
    Register_calculate_by_et(StdArray<N>, array, StdArray<N>, array, StdArray<N>, array);
    // vector&vector-> array
    Register_calculate_by_et(StdVector, vector, StdVector, vector, StdArray<N>, array);
    // array&vector-> array
    Register_calculate_by_et(StdArray<N>, array, StdVector, vector, StdArray<N>, array);
    // vector&array-> array
    Register_calculate_by_et(StdVector, vector, StdArray<N>, array, StdArray<N>, array);

    // array&array -> vector
    Register_calculate_by_et(StdArray<N>, array, StdArray<N>, array, StdVector, vector);
    // vector&vector-> vector
    Register_calculate_by_et(StdVector, vector, StdVector, vector, StdVector, vector);
    // array&vector-> vector
    Register_calculate_by_et(StdArray<N>, array, StdVector, vector, StdVector, vector);
    // vector&array-> vector
    Register_calculate_by_et(StdVector, vector, StdArray<N>, array, StdVector, vector);
}

void RegisterAll() {
    RegisterBenchmark<8>();
    RegisterBenchmark<32>();
    RegisterBenchmark<128>();
    RegisterBenchmark<512>();
    RegisterBenchmark<2048>();
}

// BENCHMARK_MAIN();
int main(int argc, char **argv) {
    char arg0_default[] = "benchmark";
    char *args_default = arg0_default;
    if (!argv) {
        argc = 1;
        argv = &args_default;
    }
    ::benchmark ::Initialize(&argc, argv);

    {
        RegisterAll();
    }

    if (::benchmark ::ReportUnrecognizedArguments(argc, argv))
        return 1;
    ::benchmark ::RunSpecifiedBenchmarks();
    ::benchmark ::Shutdown();
    return 0;
}
