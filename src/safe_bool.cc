/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/09 17:08:52
# Desc   :
########################################################################
通过隐式转换将类型 T 转换为 bool 类型，可以用作 if 判断，但是也会导致两个不同类型的 T 都
转换成 bool 进行比较，这可能不是我们想要的，我们想要禁止两个不同类型 T 转换成 bool 进行比较
虽然通过隐式转换成 bool 可以对实例进行为 true/false 判断，但是我们希望禁止:
1. 同类型比较 (t1 == t2)
2. 不同类型比较 (t1 == u1)
3. 隐式转换后有赋值给其他类型 (int i = t1;)
4. 隐式转换后进行计算 (int i = t1 + 1;)
5. 如果通过转换成 void* 的方式，应该要避免内存操作，如 delete t1
6. 最好能避免直接和 true/false 进行比较，如 if (t1 == true)，希望直接使用 if (t1) 这样的表达方式

c++11 explicit conversion operator
explicit bool() {}
@refs:
1. https://zhuanlan.zhihu.com/p/30173442
2. https://www.boost.org/doc/libs/1_65_1/boost/core/explicit_operator_bool.hpp
  boost 在支持explicit operator bool的环境使用explicit operator bool，否则使用Safe Bool Idiom。
*/

#include <print>

namespace bad_case {
class Testable {
public:
  operator bool() const {
    return false;
  }
};  // class Testable
class AnotherTestable {
public:
  operator bool() const {
    return true;
  }
};  // class AnotherTestable

void run() {
  Testable a;
  AnotherTestable b;
  if (a == b) {
    std::println("a is equal to b");
  }
  if (a < 0) {
    std::println("a is less than 0");
  }
}
}  // namespace bad_case

namespace solution_1 {
/*
// 在 operator==/!= 调用 this_type_does_not_support_comparisons()，编译器会出现 error
class Testable {
  bool ok_;
  using bool_type = void(Testable::*)() const;
  void this_type_does_not_support_comparisons() const {}
public:
  explicit Testable(bool b=true): ok_(b) {}
  operator bool_type() const {
    return ok_ ? &Testable::this_type_does_not_support_comparisons : nullptr;
  }
};  // class Testable

template <class T>
bool operator!=(const Testable& lhs, const T&) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}
template <class T>
bool operator==(const Testable& lhs, const T&) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}
class AnotherTestable {
public:
  operator bool() const {
    return true;
  }
};  // class AnotherTestable
void run() {
  Testable t1;
  AnotherTestable t2;
  if (t1) {
    std::println("t1 is true");
  }
  // if (t1 == t2) {}
  // if (t1 < 0) {}
}
*/

}  // namespace solution_1


namespace solution_2 {
class safe_bool_base {
public:
  using bool_type = void(safe_bool_base::*)() const;
  void this_type_does_not_support_comparisons() const {}
protected:
  safe_bool_base() {}
  safe_bool_base(const safe_bool_base&) {}
  safe_bool_base& operator=(const safe_bool_base&) {
    return *this;
  }
  ~safe_bool_base() {}
};  // class safe_bool_base

template <class T=void>
class safe_bool : private safe_bool_base {
public:
  operator bool_type() const {
    return (static_cast<const T*>(this))->boolean_test() ?
        &safe_bool_base::this_type_does_not_support_comparisons : 0;
  }
protected:
  ~safe_bool() {}
};  // class safe_bool

template <>
class safe_bool<void> : private safe_bool_base {
public:
  operator bool_type() const {
    return boolean_test() ?
        &safe_bool_base::this_type_does_not_support_comparisons : 0;
  }
protected:
  virtual bool boolean_test() const = 0;
  virtual ~safe_bool() {}
};  // class safe_bool<void>

template <class T>
bool operator==(const safe_bool<T>& lhs, bool b) {
  return b == static_cast<bool>(lhs);
}

template <class T>
bool operator==(bool b, const safe_bool<T>& lhs) {
  return b == static_cast<bool>(lhs);
}

template <class T, typename U>
bool operator==(const safe_bool<T>& lhs, const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}

template <class T, class U>
bool operator!=(const safe_bool<T>& lhs,const safe_bool<U>& rhs) {
  lhs.this_type_does_not_support_comparisons();
  return false;
}

class Testable_with_virtual : public safe_bool<> {
public:
  virtual ~Testable_with_virtual() {}
protected:
  virtual bool boolean_test() const {
    return true;
  }
};  // class Testable_with_virtual

class Testable_without_virtual : public safe_bool<Testable_without_virtual> {
public:
  bool boolean_test() const {
    return false;
  }
};  // class Testable_without_virtual


void run() {
  Testable_with_virtual t1, t2;
  Testable_without_virtual p1, p2;
  if (t1) {}
  if (false == p1) {
    std::println("p1 == false");
  }
  //if (p1 == p2) {

  //}
  ////if (t1 != t2) {

  //}
}

}  // namespace solution_2

// c++11 显示转换操作符
namespace solution_3 {
struct Testable {
  explicit operator bool() const {
    return false;
  }
};  // struct Testable

void run() {
  Testable a, b;
  // https://blog.csdn.net/bingqingsuimeng/article/details/80419064
  // 作为条件判断时，可以隐式转换，如 if (a),  for(; a; ), while (a)
  if (a) {}
  // no: 因为 a 和 b 的比较需要显示转换成 bool 才能比较
  // if (a == b) {}
}
}  // namespace solution_3

int main() {
  bad_case::run();
  solution_2::run();
  return 0;
}

