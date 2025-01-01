/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/01 14:49:55
# Desc   :
########################################################################
通过使用 iterator_pair 来转递容器数据，以便用于构造其他容器
*/

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <list>
#include <numeric>
#include <print>
#include <set>
#include <vector>

#include "utils/main_decorator.h"

template <class Iterator>
void print(Iterator begin, Iterator end) {
  std::println(
      "{}", std::accumulate(begin, end, std::string{},
                [](const std::string& pre, const auto& x) { return pre + std::to_string(x); }));
}

namespace coercion_by_member_template {
template <class T>
class Vector {
public:
  Vector(std::initializer_list<T> init) : size_(init.size()), t_(new T[size_]) {
    std::copy(init.begin(), init.end(), t_);
  }
  Vector(const Vector<T>& rhs) : size_(rhs.size()), t_(new T[size_]) {
    std::copy(rhs.t_, rhs.t_ + size_, t_);
  }
  template <class U>
  Vector(const Vector<U>& rhs) : size_(rhs.size()), t_(new T[size_]) {
    std::copy(rhs.t_, rhs.t_ + rhs.size_, t_);
  }
  Vector& operator=(const Vector<T>& rhs) {
    if (this == &rhs) {
      return *this;
    }
    auto old = t_;
    t_ = new T[rhs.size()];
    std::copy(rhs.t_, rhs.t_ + rhs.size(), t_);
    size_ = rhs.size();
    delete old;
    return *this;
  }

  // Vector 并不知道 List/Set 的接口（并不一定是 std::list/std::set，可能是某个自定义的 array）
  // Vector(const List<T>& rhs);
  // Vector(const Set<T>& rhs);
  // ...
  ~Vector() {
    if (t_ != nullptr) delete[] t_;
  }

  size_t size() const { return size_; }
  void print() const { ::print(t_, t_ + size_); }

private:
  size_t size_{0};
  T* t_{nullptr};
};  // class Vector
}  // namespace coercion_by_member_template

namespace iterator_pair {
template <class T>
class Vector {
public:
  Vector(std::initializer_list<T> init) : size_(init.size()), t_(new T[size_]) {
    std::copy(init.begin(), init.end(), t_);
  }
  Vector(const Vector<T>& rhs) : size_(rhs.size()), t_(new T[size_]) {
    std::copy(rhs.t_, rhs.t_ + rhs.size_, t_);
  }
  // 通过接收 iterator begin/end 的方式来构造容器
  template <class InputIterator>
  Vector(InputIterator begin, InputIterator end)
      : size_(std::distance(begin, end)), t_(new T[size_]) {
    std::copy(begin, end, t_);
  }
  ~Vector() {
    if (t_ != nullptr) delete[] t_;
  }
  size_t size() const { return size_; }
  void print() const { ::print(t_, t_ + size_); }

private:
  size_t size_{0};
  T* t_{nullptr};
};  // class Vector
}  // namespace iterator_pair

void run() {
  {
    using namespace coercion_by_member_template;
    Vector<int> v1{1, 2, 3, 4, 5};
    Vector<int> v2{5, 4, 3, 2, 1};
    Vector<int> v3(v1);
    v3.print();
    v3 = v2;
    v3.print();
    std::list<int> l{1, 2, 3, 4, 5};
    // no
    // Vector<int> v4(l);
  }
  std::println();
  {
    using namespace iterator_pair;
    Vector<int> v1{1, 2, 3, 4, 5};
    Vector<int> v3(v1);
    v3.print();
    std::list<int> l{1, 2, 3, 4, 5};
    Vector<int> v4(l.begin(), l.end());
    v4.print();
    std::set<int> s{5, 4, 3, 2, 1};
    Vector<int> v5(s.begin(), s.end());
    v5.print();
  }
  {
    char buf[] = {'A', 'B', 0, 'C', 0, 'D'};
    std::string str(buf);
    std::println("str: {} {}", str, str.length());
    assert("AB" == str);
    std::string str2(buf, sizeof(buf));
    assert("AB\0C\0D" == str);
    std::string str3(buf, buf + sizeof(buf) / sizeof(buf[0]));
    assert("AB\0C\0D" == str);
  }
}
int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}
