/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/09 15:52:01
# Desc   :
########################################################################
当函数返回类型依赖于模板参数，调用函数时可能需要填写该参数，如下 bad_case::get_n，返回
参数取决于模板参数 Container，当调用该函数时，需要写成 Container c = get_n<Container>(n)
这样写起来比较麻烦，特别时 Container 的名字比较长的时候

@solution:
通过类的隐式转换函数来减少参数填写，隐式转换的模板参数取决于转换后承接该实例的类型，比如
Container c = get_n(n)，如果 get_n 可以进行隐式转换，那么转换成什么类型取决于 c 的类型

另外，在 nullptr.cc 中也使用类隐式转换，可以将 nullptr 的实例转换成任何类型的指针 T*
*/

#include <cstddef>
#include <list>
#include <print>
#include <vector>
#include "utils/main_decorator.h"
namespace bad_case {
template <class Container>
Container get_n(size_t n) {
  Container c;
  for (size_t i = 0; i < n; ++i) {
    c.insert(c.end(), i);
  }
  return c;
}

void run() {
  std::list<int> l = get_n<std::list<int>>(10);
  std::vector<long> v = get_n<std::vector<long>>(100);
}
}  // namespace bad_case


namespace solution {
class get_n {
  size_t count;
public:
  get_n(size_t n) : count(n) {}
  template <class Container>
  operator Container() {
    std::println("隐式转换");
    Container c;
    for (size_t i = 0; i < count; ++i) {
      c.insert(c.end(), i);
    }
    return c;
  }
};  // class get_n

void run() {
  std::list<int> l = get_n(10);
  std::vector<long> v = get_n(100);
}
}  // namespace solution
int main() {
  utils::MainDecorator::Access();
  bad_case::run();
  solution::run();
  return 0;
}
