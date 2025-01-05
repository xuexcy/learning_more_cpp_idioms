/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 20:18:32
# Desc   :
########################################################################
通过宏，使用 goto 来跳出某个代码块
*/

#include <print>
#include "utils/main_decorator.h"

#define NAMED_LOOP(loop_name) \
  goto loop_name; \
  loop_name##_skip: if (0) \
  loop_name:

#define BREAK(loop_name) \
  goto loop_name##_skip

void run() {
  int sum{0};
  // 注意，后面不能有分号
  // no:   NAMED_LOOP(outer); <---  分号
  NAMED_LOOP(outer)
  for (int i = 1; true; ++i) {
    NAMED_LOOP(inner)
    for (int j = 1; true; ++j) {
      std::println("i: {}, j: {}", i, j);
      sum += i;
      sum += j;
      if (i % 2 == 0) BREAK(outer);
      if (j % 3 == 0) BREAK(inner);
    }
  }
  std::println("sum: {}", sum);
}

void run_after_macro_expand() {
  int sum{0};
  goto outer;
  outer_skip:
  if (0)
    outer:
    for (int i = 1; true; ++i) {
      goto inner;
      inner_skip:
      if (0)
        inner:
        for (int j = 1; true; ++j) {
          std::println("i: {}, j: {}", i, j);
          sum += i;
          sum += j;
          if (i % 2 == 0) goto outer_skip;
          if (j % 3 == 0) goto inner_skip;
        }
    }
  std::println("sum: {}", sum);
}

int main() {
  utils::MainDecorator::Access();
  run();
  run_after_macro_expand();
  return 0;
}

