/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/10 19:08:53
# Desc   :
########################################################################
在 handle body(pointer_to_implementation.cc pointer_to_implementation_deps/) 中,
handle 类(也就是 class BookBase) 中的成员函数的定义都是调用 body 类(也就是 class Impl)的同名函数，
如果这样的函数很多，那么 handle 可能需要定义很多和 body 类同名的函数

@solution: 在 handle 类中通过重载 operator-> 和 operator* 来获取 body 类的指针，只是使用该指针调用函数
*/

#include <print>
#include <string>
#include "utils/main_decorator.h"
namespace solution {
class Book {
public:
  class Impl;
  Book();
  ~Book();
  Impl* operator->() {
    return impl_;
  }
  const Impl* operator->() const {
    return impl_;
  }
  Impl& operator*() {
    return *impl_;
  }
  const Impl& operator*() const {
    return *impl_;
  }
private:
  Impl* impl_{nullptr};
  friend const Book::Impl& get(const Book& b);
};  // class Book

class Book::Impl {
public:
  Impl(): title_("Hail, Hydra!"), contents_("菩萨保佑") {}
  void print() {
    std::println("title: {}, contents: {}", title_, contents_);
  }
private:
  std::string title_;
  std::string contents_;
};

Book::Book(): impl_(new Book::Impl()) {}
Book::~Book() { delete impl_; }

// 通过 get 函数获取 impl_ 也可以
const Book::Impl& get(const Book& b) {
  return *b.impl_;
}

void run() {
  Book b;
  b->print();
  // 这种写法不是很好看，如果 handle 类不含有指针语意(比如 std::shared_ptr)，那么可以不用提供 operator*()
  (*b).print();
}
}  // namespace solution

int main() {
  utils::MainDecorator::Access();
  solution::run();
  return 0;
}


