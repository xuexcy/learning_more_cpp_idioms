/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/12/04 23:32:18
# Desc    :
########################################################################
*/

#include "benchmark/benchmark.h"
#include "calculation_benchmark_deps.h"

template <size_t N>
void RegisterBenchmark() {
  // array by operator overload
  benchmark::RegisterBenchmark(std::format("BM_calculate_by_operator_overload array size: {}", N),
      BM_calculate_by_operator_overload<N, StdArray<N>>);
  // array by expression
  benchmark::RegisterBenchmark(std::format("BM_calculate_by_expression array size: {}", N),
      BM_calculate_by_expression<N, StdArray<N>>);
  // array&array -> array
  Register_calculate_by_et(StdArray<N>, array, StdArray<N>, array, StdArray<N>, array);
}

void RegisterAll() {
  RegisterBenchmark<1>();
  RegisterBenchmark<2>();
  RegisterBenchmark<4>();
  RegisterBenchmark<8>();
  RegisterBenchmark<20>();
  RegisterBenchmark<32>();
  RegisterBenchmark<40>();
  RegisterBenchmark<50>();
  RegisterBenchmark<64>();
  RegisterBenchmark<80>();
  RegisterBenchmark<100>();
  RegisterBenchmark<128>();
  RegisterBenchmark<180>();
  RegisterBenchmark<256>();
  RegisterBenchmark<400>();
  RegisterBenchmark<512>();
  RegisterBenchmark<750>();
  RegisterBenchmark<1024>();
  RegisterBenchmark<1500>();
  RegisterBenchmark<2048>();
  RegisterBenchmark<3000>();
  RegisterBenchmark<4096>();
  RegisterBenchmark<6000>();
  RegisterBenchmark<8192>();
  RegisterBenchmark<12000>();
  RegisterBenchmark<16384>();
}

// BENCHMARK_MAIN();
int main(int argc, char** argv) {
  char arg0_default[] = "benchmark";
  char* args_default = arg0_default;
  if (!argv) {
    argc = 1;
    argv = &args_default;
  }
  ::benchmark ::Initialize(&argc, argv);

  {
    RegisterAll();
  }

  if (::benchmark ::ReportUnrecognizedArguments(argc, argv)) return 1;
  ::benchmark ::RunSpecifiedBenchmarks();
  ::benchmark ::Shutdown();
  return 0;
}
