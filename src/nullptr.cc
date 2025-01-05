/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 21:55:22
# Desc   :
########################################################################
*/

// 通过隐式转换的方式，将 nullptr_t 的实例转换成指针
// 1. 类型指针
// 2. 函数指针
class nullptr_t {
public:
  template <class T>
  inline operator T*() const { return 0; }
  // class C 的函数指针，返回类型为 T
  template <class C, class T>
  inline operator T C::*() const { return 0; }
private:
  void operator&() const;
};  // class nullptr_t
const nullptr_t NULLPTR{};

class A {
public:
  void hi() const {}
  void he(int) {}
};  // class A
void run() {
  int* a = NULLPTR;
  void (A::*func_ptr)() const = NULLPTR;
  void (A::*he)(int) = NULLPTR;

  func_ptr = &A::hi;
  he = &A::he;
}

int main() {
  run();
  return 0;
}
