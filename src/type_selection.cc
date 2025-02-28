/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/02/28 20:17:00
# Desc   :
########################################################################
在编译期，根据一些静态条件确定数据类型，即 c++ 11 的 std::conditional
*/
#include <type_traits>
#include "utils/main_decorator.h"

// c++ 11 std::conditional
template <bool, class L, class R>
struct IF {
  using type = R;
};  // struct IF
template <class L, class R>
struct IF<true, L, R> {
  using type = L;
};  // struct IF

void run() {
  static_assert(std::is_same_v<IF<true, int, long>::type, int>);
  static_assert(std::is_same_v<IF<false, int, long>::type, long>);
}


namespace queue {
// 通过 IF ，根据 CAPACITY 的值选择 size 的类型
template <class T, unsigned int CAPACITY>
struct Queue {
  T array[CAPACITY];
  using size = IF<(
    CAPACITY <= 256),
    unsigned char,
    typename IF<
      (CAPACITY <= 65536),
      unsigned short,
      unsigned int
    >::type
  >::type;
};  // class Queue
void run() {
  static_assert(std::is_same_v<Queue<int, 127>::size, unsigned char>);
  static_assert(std::is_same_v<Queue<int, 65436>::size, unsigned short>);
  static_assert(std::is_same_v<Queue<int, 70000>::size, unsigned int>);
}
}  // namespace queue

int main() {
  utils::MainDecorator::Access();
  run();
  queue::run();
  return 0;
}


