/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 22:06:04
# Desc   :
########################################################################
通过 non-virtual interface，可以在基类中做一些校验、检测、限制（pre- and post- conditions）

实现步骤:
1. Base 类 public 的接口，实现基础功能，并调用 private 的 virtual function，一个 public 接口对应一个 private virtual function
2. 开发者在 Derived 类中实现 private virtual function
3. 如果 Derived 类需要调用 Base 类的 virtual 函数，可以将它设置为 protected
4. Base 中的要有 public virtual destructor
5. 用户实例化 Derived 类并使用 Base 类中 public 接口

但是，如果 Base 的 public 改变了实现方式，可能会导致一些问题，比如 set::Set 中的 all_all 的实现改成
while (begin != end) {
  add(*begin);
  ++begin;
}
add_all_impl(begin, end);

那么会导致计数错误，也就是一个 public 接口调用另一个 public 接口，可能会出错，所以尽量时 public 接口去调用
private virtual function
*/

#include <iostream>
#include <mutex>
#include <ostream>
#include <print>
#include <set>
#include <sstream>
#include <vector>
#include "utils/main_decorator.h"

namespace set {
class Set {
public:
  virtual ~Set() {
    std::println("set elements: {}", s_);
  }
  void add(int i) {
    s_.insert(i);
    add_impl(i);
  }
  void add_all(int* begin, int* end) {
    s_.insert(begin, end);
    add_all_impl(begin, end);
  }

private:
  virtual void add_impl(int i) = 0;
  virtual void add_all_impl(int* begin, int* end) = 0;
private:
  std::set<int> s_;
};  // class Set

class CountingSet : public Set {
public:
  virtual ~CountingSet() {
    std::println("count: {}", count_);
    count_ = 0;
  }
private:
  virtual void add_impl(int) override {
    ++count_;
  }
  virtual void add_all_impl(int* begin, int* end) override {
    count_ += std::distance(begin, end);
  }
private:
  int count_{0};
};  // class CountingSet

void run() {
  CountingSet s;
  s.add(1);
  s.add(2);
  std::vector<int> v{1, 2, 3};
  s.add_all(&v[0], &v[0] + v.size());

}

}  // namespace set

namespace reader_writer {
class Base {
public:
  virtual ~Base() = default;
  void read_from(std::istream& i) {
    std::unique_lock<std::mutex> lock(mtx_);
    read_from_impl(i);
  }
  void write_to(std::ostream& o) {
    std::unique_lock<std::mutex> lock(mtx_);
    write_to_impl(o);
  }
private:
  virtual void read_from_impl(std::istream& i) = 0;
  virtual void write_to_impl(std::ostream& o) = 0;
private:
  std::mutex mtx_;
};  // class Base

class AReaderWriter : public Base {
private:
  virtual void read_from_impl(std::istream& i) override {
    std::string s;
    i >> s;
    data_.insert(data_.end(), s.begin(), s.end());
  }
  virtual void write_to_impl(std::ostream& o) override {
    for (auto& ch: data_) {
      o << ch;
    }
    o << std::endl;
  }
private:
  std::vector<char> data_;
};  // class AWriter

class BReaderWriter : public Base {
private:
  virtual void read_from_impl(std::istream& i) override {
    std::string s;
    i >> s;
    for (int idx = 0; idx + 3 < s.length(); ++idx) {
      data_.insert(data_.end(), &s[idx], &s[idx + 3]);
    }
  }
  virtual void write_to_impl(std::ostream& o) override {
    for (int i = 0; i < data_.size(); i += 3) {
      o << data_[i] << data_[i+1] << data_[i+2] << std::endl;
    }
  }
private:
  std::vector<char> data_;
};  // class AWriter


void run() {
  {
    std::stringstream ss;
    ss << ("abcdefg");
    AReaderWriter a;
    a.read_from(ss);
    a.write_to(std::cout);
  }
  {
    std::stringstream ss;
    ss << ("abcdefg");
    BReaderWriter b;
    b.read_from(ss);
    b.write_to(std::cout);
  }
}

}  // namespace reader_writer

int main() {
  utils::MainDecorator::Access();
  set::run();
  reader_writer::run();
  return 0;
}
