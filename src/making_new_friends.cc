/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyunxue@gmail.com
# Date   : 2025/01/02 15:08:58
# Desc   :
########################################################################
在类模板中声明友元函数，如果是重载非模板 operator，比如 operator<<，则需要将定义和声明都放在类模板中，如下 class Foo_2
*/

#include <iostream>
#include <ostream>
#include <print>
#include "cpp_utils/util.h"

template <typename T>
class Foo_1 {
  T value;
public:
  Foo_1(const T& t): value(t) {}
  friend std::ostream& operator<<(std::ostream& os, const Foo_1& foo);
};  // class Foo
template <typename T>
std::ostream& operator<<(std::ostream& os, const Foo_1<T>& foo) {
  // 这并不是 Foo_1 中的 operator<< 的定义，而是一个全局的 operator<<
  PRINT_CURRENT_FUNCTION_NAME;
  return os << foo.value;
}
void run_foo_1() {
  PRINT_CURRENT_FUNCTION_NAME;
  Foo_1<int> foo_1(42);
  // 链接错误
  // 因为标准库中定义的 << 不是一个模板，所以我们重载(也许都不应该叫重载)的模板 << 无法被调用，导致重载的 << 可以编译但无法链接
  // std::cout << foo << std::endl;

  // 链接错误
  // 这里想要调用 class Foo_1 中的 operator<<，但是无法调用，因为没有定义(在紧跟 class Foo_1 后的那个
  // operator<< 其实不是 class Foo_1 里面的 friend operator<< 的定义)
  // operator<<(std::cout, foo_1);

  // 编译错误: 'value' is a private member of 'Foo_1<int>'
  // 在没有 :: 修饰符的情况下，编译器会找到 class Foo_1 里面的 operator<<，但是它只有声明，没有定义，会发生链接错误
  // 这里我们加上 :: 修饰符，就会调用全局的 operator<<，但是 foo.value 是 private，所以会发生编译错误
  // ::operator<<(std::cout, foo_1);
}

// @solution: 将 operator<< 的定义和声明都放在 class Foo_2 里面
template <typename T>
class Foo_2 {
  T value;
public:
  Foo_2(const T& t): value(t) {}
  friend std::ostream& operator<<(std::ostream& os, const Foo_2& foo) {
    PRINT_CURRENT_FUNCTION_NAME;
    return os << foo.value;
  }
};  // class Foo_2

void run_foo_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  Foo_2<int> foo_2(42);
  std::cout << foo_2 << std::endl;
}

// @solution: forward declaration
//   1. 使用前置声明模板 operator<<
//   2. 在 class Foo_4 中声明特化的友元 operator<< <>
//   3. 在全局定义模板 operator<<
template <class T> class Foo_3;
template <class T> std::ostream& operator<<(std::ostream& os, const Foo_3<T>& foo);
template <typename T>
class Foo_3 {
  T value;
public:
  Foo_3(const T& t): value(t) {}
  friend std::ostream& operator<< <>(std::ostream& os, const Foo_3& foo);
};  // class Foo_3
template <class T>
std::ostream& operator<<(std::ostream& os, const Foo_3<T>& foo) {
  PRINT_CURRENT_FUNCTION_NAME;
  return os << foo.value;
}

void run_foo_3() {
  PRINT_CURRENT_FUNCTION_NAME;
  Foo_3<int> foo_3(42);
  std::cout << foo_3 << std::endl;
}


int main() {
  run_foo_1();
  run_foo_2();
  run_foo_3();
  return 0;
}
