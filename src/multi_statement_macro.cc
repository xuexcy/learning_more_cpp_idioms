/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/03 19:14:52
# Desc   :
########################################################################
通过 do {xxx; xxx} while(0) 来解决宏展开后语法错误的问题，不过这种宏不能作为函数参数
*/


#include <print>
namespace bad_case {
#define MACRO(X,Y) { ++X; --Y; }
void run() {
  int x = 10, y = 20;
  // if (x > 0)
  //   MACRO(x, y);
  // else
  //   --x;

  // // 宏展开可以看到语法错误：} 后多了一个分号
  // if (x > 0)
  // { ++x; --y;};
  // else
  //   --x;
  std::println("x: {}, y: {}", x, y);
}
}  // namespace bad_case

namespace solution {
// 编译器会将 while(0) 优化掉
#define MACRO_2(X,Y) do { ++X; --Y; } while(0)
void run() {
  int x = 10, y = 20;
  if (x > 0)
    MACRO_2(x, y);
  else
    --x;

  // 宏展开
  if (x > 0)
  do { ++x; --y;} while(0);
  else
    --x;
  std::println("x: {}, y: {}", x, y);
}
}  // namespace solution


// 上面通过 do {xxx; xxx} while(0) 的方式来解决宏展开后语法错误的问题，不过这种宏不能作为函数参数
namespace bad_case_2 {
#define MACRO_3(X, Y) do { ++X; --Y; return X + Y} while (0)
void run() {
  int x = 10, y = 20;
  // std::println("(++x) + (--y) = {}", MACRO_3(x, y));
}
}  // namespace bad_case

int main() {
  bad_case::run();
  solution::run();
  bad_case_2::run();
  return 0;
}

