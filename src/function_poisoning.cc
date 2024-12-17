/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/12/17 17:09:34
# Desc    :
########################################################################
将一些函数/功能封装起来，并使用 #pragma GCC xxx 将原有的函数隐藏起来不让用户使用
比如一些 c 或者 c++ 11 之前的 create/destroy 函数，可以封装成 RAII 的样子
*/

#include "function_poisoning_deps/foo.h"
#include "utils/main_decorator.h"

/*
 如下 foo，将 function_poisoning_deps/__details/foo.h 中的 foo* create_foo() 和 void destroy_foo(foo*)
 封装到 unique_foo 中，并使用 #pragma GCC create_foo 来禁止用户使用该函数创建 foo 实例
 */

void run() {
  // no:
  //  clangd: Attempt to use a poisoned identifier
  // foo* f1 = create_foo();

  unique_foo f2 = create_unique_foo();
}
int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}
