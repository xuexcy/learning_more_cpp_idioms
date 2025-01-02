/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/02 16:34:13
# Desc   :
########################################################################
在编译期选择调用的函数
如下 struct IF 在编译期选择了类型，struct Factorial 在编译期计算了阶乘
*/

// https://en.cppreference.com/w/cpp/types/conditional
// std::conditional_t
#include <print>
#include <type_traits>
#include <typeinfo>
template <bool, class L, class R>
struct IF {
  using type = R;
};  // struct IF
template <class L, class R>
struct IF<true, L, R> {
  using type = L;
};  // struct IF
template <bool B, class L, class R>
using IF_t = IF<B, L, R>::type;

template <int N>
struct Factorial {
  static constexpr int value = N * Factorial<N - 1>::value;
};  // struct Factorial
template <>
struct Factorial<0> {
  static constexpr int value = 1;
};  // struct Factorial


void run() {
  IF_t<true, int, double> a = 42;
  IF_t<false, int, double> b = 42;
  std::conditional_t<true, int, double> c = 42;
  std::conditional_t<false, int, double> d = 42;
  #define PRINT(x) std::println("type: {}, name: {}, value {}", typeid(x).name(), #x, x)
  PRINT(a);
  PRINT(b);
  PRINT(c);
  PRINT(d);
  std::println("Factorial<0>::value: {}", Factorial<0>::value);
  std::println("Factorial<5>::value: {}", Factorial<5>::value);
}

int main() {
  run();
  return 0;
}
