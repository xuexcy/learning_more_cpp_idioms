/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2024/12/31 18:13:56
# Desc   :
########################################################################
在编译时，使用 Int2Type 的方式，通过 int 值来区分不同的类型（也可以通过 enable_if 的方式实现）
@solution:
1. Int2Type
2. enable_if
3. constexpr
*/

#include <print>
#include <type_traits>
#include "utils/main_decorator.h"
namespace int_to_type {
template <int I>
struct Int2Type {
};  // struct Int2Type

template <class T, unsigned int N>
class Array {
  enum class Algo {
    NO_OP,
    INSERTION_SORT,
    QUICK_SORT
  };
  static constexpr Algo kAlgo = (N == 0) ? Algo::NO_OP :
      (N == 1) ? Algo::NO_OP :
      (N < 50) ? Algo::INSERTION_SORT : Algo::QUICK_SORT;
  void sort(Int2Type<static_cast<int>(Algo::NO_OP)>) {
    std::println("no_op");
  }
  void sort(Int2Type<static_cast<int>(Algo::INSERTION_SORT)>) {
    std::println("insertion_sort");
  }
  void sort(Int2Type<static_cast<int>(Algo::QUICK_SORT)>) {
    std::println("quick_sort");
  }
public:
  void sort() {
    sort(Int2Type<static_cast<int>(kAlgo)>());
  }
};  // class Array
}  // namespace int_to_type

namespace enable_if {
template <unsigned int N, typename std::enable_if_t<(N == 0 || N == 1), bool> = true>
void sort() {
  std::println("no_op");
}
template <unsigned int N, typename std::enable_if_t<(N > 1 && N < 50), bool> = true>
void sort() {
  std::println("insertion_sort");
}
template <unsigned int N, typename std::enable_if_t<N >= 50, bool> = true>
void sort() {
  std::println("quick_sort");
}
template <class T, unsigned int N>
class Array {
public:
  void sort() {
    enable_if::sort<N>();
  }
};  // class Array
}  // namespace enable_if

namespace use_constexpr {
template <unsigned int N, typename std::enable_if_t<(N == 0 || N == 1), bool> = true>
void sort() {
  std::println("no_op");
}
template <unsigned int N, typename std::enable_if_t<(N > 1 && N < 50), bool> = true>
void sort() {
  std::println("insertion_sort");
}
template <unsigned int N, typename std::enable_if_t<N >= 50, bool> = true>
void sort() {
  std::println("quick_sort");
}
template <class T, unsigned int N>
class Array {
public:
  void sort() {
    if constexpr (N <= 1) {
      std::println("no_op");
    } else if constexpr (N < 50) {
      std::println("insertion_sort");
    } else {
      std::println("quick_sort");
    }
  }
};  // class Array
}  // namespace use_constexpr

void run() {
  {
    using namespace int_to_type;
    std::println("int_to_type");
    Array<int, 0> arr_0;
    arr_0.sort();
    Array<int, 1> arr_1;
    arr_1.sort();
    Array<double, 15> arr_15;
    arr_15.sort();
    Array<double, 50> arr_50;
    arr_50.sort();
  }
  {
    std::println("\nenable_if");
    using namespace enable_if;
    Array<int, 0> arr_0;
    arr_0.sort();
    Array<int, 1> arr_1;
    arr_1.sort();
    Array<double, 15> arr_15;
    arr_15.sort();
    Array<double, 50> arr_50;
    arr_50.sort();
  }
  {
    using namespace use_constexpr;
    std::println("\nuse_constexpr");
    Array<int, 0> arr_0;
    arr_0.sort();
    Array<int, 1> arr_1;
    arr_1.sort();
    Array<double, 15> arr_15;
    arr_15.sort();
    Array<double, 50> arr_50;
    arr_50.sort();
  }
}

namespace node {
template <int I>
struct Int2Type {
  enum { value = I };
  typedef int value_type;
  typedef Int2Type<I> type;
  typedef Int2Type<I+1> next;
  typedef Int2Type<I-1> previous;
};  // Int2Type
}  // namespace node

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}

