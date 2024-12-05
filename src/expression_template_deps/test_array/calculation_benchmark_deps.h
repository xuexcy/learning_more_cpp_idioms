/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/12/04 23:31:18
# Desc    :
########################################################################
*/
#include "benchmark/benchmark.h"
#include "calculation_deps.h"
#include "expression_template_deps/array/operator.h"

template <size_t N, ArrayLike A, ArrayLike B = A>
auto init_array() {
  A a = get_array<A>(N);
  B b = get_array<B>(N);
  init_array(a, b);
  return std::make_pair(a, b);
}

template <size_t N, ArrayLike A, ArrayLike B = A>
static void BM_calculate_by_operator_overload(::benchmark::State& state) {
  auto [a, b] = init_array<N, A, B>();
  for (auto _ : state) {
    auto res = array_operator_overload::calculate(a, b);
    ++res[0];
  }
}

template <size_t N, ArrayLike A, ArrayLike B = A>
static void BM_calculate_by_expression(::benchmark::State& state) {
  auto [a, b] = init_array<N, A, B>();
  for (auto _ : state) {
    auto res = calculate_by_expression(a, b);
    ++res[0];
  }
}

template <size_t N, ArrayLike A, ArrayLike B = A>
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

#define Register_calculate_by_et(AType, a_name, BType, b_name, ResType, res_name)              \
  benchmark::RegisterBenchmark(                                                                \
      std::format("BM_calculate_by_et res={} {}&{} size: {}", #res_name, #a_name, #b_name, N), \
      BM_calculate_by_et<N, AType, BType, ResType>)
