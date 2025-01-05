/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 15:41:50
# Desc   :
########################################################################
一个 global object 可能被多个不同编译单元的其他 global object
使用，需要确保在第一次使用这个实例时初始化。 其实不一定要使用 global object，可以看看
construct_on_first_use.cc ，通过使用 static class function 获取 object
的方式来做初始化，并由程序在结束时回收内存

@solution:
通过 class Initializer + counter 来构造和析构一个 global object

针对 nifty_counter_deps/stream.h 进行分析
1. 不同编译单元 #include "stream.h" 后，都会对 static StreamInitializer stream_initializer 进行构造
  - 初始化 stream_initializer 使用链接到 lib_stream (即 stream.cpp) 中的  static int nifty_counter，
根据其是否为 0 来决定是否使用 placement new 来构造 Stream& stream
2. 程序在退出时，每个编译单元中的 stream_initializer 都会析构一次，并在 nifty_counter 归 0 时，将
stream 析构

根据 stdout 可以看到，nifty_counter 最大值为 4，因为一共有四个编译单元(stream, a, b, main)，
每个单元都有一个 stream_initializer
*/

#include "nifty_counter_deps/a.h"
#include "nifty_counter_deps/b.h"
#include "nifty_counter_deps/stream.h"
#include "utils/main_decorator.h"

void run() {
  A a;
  B b;
  a.use_global_stream();
  b.use_global_stream();
  ::stream.say_hi();
}

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}
