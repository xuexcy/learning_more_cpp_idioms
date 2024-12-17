/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyunxue@gmail.com
# Date    :   2024/12/17 14:48:38
# Desc    :
########################################################################
限定类不可以被继承，使用 c++ 11 关键字 final
*/

#include <print>
#include "utils/main_decorator.h"

// virtual 继承 + 私有构造函数
// C++ rule: the constructor (and destructor) of a virtually inherited class is called directly by
// the derived-most class
// 构造函数和析构函数被最后继承的类直接调用
// 1. 这种方法只能限制构造函数和对象实例化，继承类还是可以调用基类的 static 函数
// 2. 还是可以正常编写继承代码，只是在实例化时会编译出错
namespace solution_1 {
class MakeFinal {
  MakeFinal() = default;
  friend class A;
  friend class B;
};  // MakeFinal

// yes
class A : virtual MakeFinal {};  // class A

// no
class B : public MakeFinal {};  // class B

class C : public A {
public:
  // no
  // C(int c): c_(c) {}
  void print_hi() { std::println("hi"); }
  static void say_hi() { std::println("hi, I am C"); }

private:
  int c_{0};
};  // class C

class D : public B {
public:
  D() = default;
};  // class D

void run() {
  A a;
  B b;
  // no:
  //   Default constructor of 'C' is implicitly deleted because base class 'MakeFinal' has an
  //   inaccessible default constructor
  // C c;

  C::say_hi();
  D d;
}

}  // namespace solution_1

// c++ 11: final
// 相比于 solution_1，class C 继承 A 就会无法编译，solution_1 里还是可以继承 A，只是无法编写构造函数和实例化对象
namespace solution_2 {
class A final {
};  // class A
// no
// class C : public A {
// };  // class C
}  // namespace solution_2

int main() {
  utils::MainDecorator::Access();
  solution_1::run();
  return 0;
}
