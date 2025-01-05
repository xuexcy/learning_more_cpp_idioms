/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 19:51:22
# Desc   :
########################################################################
1. 当函数参数较多时，用户传递参数可能需要记住传递顺序
2. 但最后几个参数是默认参数时，比如void func(int x, double y, float z, int a=1, double b=2, float c=3)，
此时想要传入 c 的参数值，需要将前面参数的默认值都写上，func(1, 2, 8)，对于填写前面几个参数的默认值也比较麻烦
@solution:
将参数放到一个代理类中，并设置 set 接口来指定参数值
*/

#include <print>
#include "utils/main_decorator.h"
struct Parameters {
  int x;
  double y;
  float z;
  int a{1};
  double b{2};
  float c{3};
  Parameters& set_x(int _x) { x = _x; return *this; }
  Parameters& set_y(double _y) { y = _y; return *this; }
  Parameters& set_z(float _z) { z = _z; return *this; }
  Parameters& set_a(int _a) { a = _a; return *this; }
  Parameters& set_b(double _b) { b = _b; return *this; }
  Parameters& set_c(float _c) { c = _c; return *this; }
};  // struct Parameters

void foo(int x, double y, float z, int a=1, double b=2, float c=3) {
  std::println("x: {}, y: {}, z: {}, a: {}, b: {}, c: {}", x, y, z, a, b, c);
}

void bar(const Parameters& p) {
  std::println("x: {}, y: {}, z: {}, a: {}, b: {}, c: {}", p.x, p.y, p.z, p.a, p.b, p.c);
}

void run() {
  foo(1, 2, 3, 1, 2, 4);  // 为了修改参数 c，必须填写参数 a、b 的默认值
  bar(Parameters().set_x(1).set_y(2).set_z(3).set_c(4));
}

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}



