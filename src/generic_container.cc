/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/12/17 19:11:17
# Desc    :
########################################################################
对比 bad_case::Stack 和 solution::Stack，可以发现 bad_case::Stack 要求 T 具有
默认构造函数和拷贝赋值运算符，但 class X 并没有或无法调用(private)这两个函数，主要问题
在于 bad_case::Stack 在构造时使用了 new T[size]
  1. 由于构造时就 new array，这样就只能调用 T 的默认(无参)构造函数，但是 T 可能并没有这个函数，我们不能假定或要求 T 有；
    另外，没有必要提前 new 那么多(size 个) T 的实例
  2. 由于构造时就 new array，所以在 push 时，调用了 T 的拷贝赋值运算符(因为所需的内存空间已经申请好了，而且对应地址
    上也构造了一个默认实例)，但是 X 的拷贝赋值运算符是 private 的，所以无法调用
    无法调用；另外，也没有必要要求 T 一定有 copy assignment operator
  3. 由于构造时就 new array，所以只能都对应 Stack 析构时才调用了 delete array，而不是在 pop 时调用，这样
在 solution::Stack 中，对于上诉问题，通过 operator new/delete 和 placement new 来解决
  1. 在构造时通过 operator new 申请内存空间，而没有构造 T 的实例，这样就不需要 T 有默认构造函数
  2. 在 push 时，通过 placement new 来构造 T 的实例，这样就不需要 T 有 copy assignment operator
  3. 在 pop 时，通过主动调用析构函数来析构实例，而不用使用 delete 来释放实例的内存，以供下一个 push 进行的实例使用
  4. 在析构时，通过 operator delete 来释放内存空间
综上，解决办法就是，将 Stack 中实例的内存申请与释放放到构造和析构函数，对于 T 的实例而言，将其构造与析构放到 push 和
pop 函数，这样分离内存问题和实例问题，设计更加合理。
*/

#include <algorithm>
#include <cstddef>
#include <print>
#include "utils/main_decorator.h"

class X {
public:
  X(int x): x_(x) {}
private:
  X& operator=(const X& x) {
    x_ = x.x_;
    return *this;
  }
private:
  int x_{0};
};  // class X

namespace bad_case {
/*
T 的要求:
1. 默认构造函数
2. 拷贝赋值运算符
3. 拷贝构造函数
4. 不抛异常的析构函数
*/
template <class T>
class Stack {
public:
  Stack(size_t size): size_(size), array_(new T[size_]) {}  // 1. default constructor
  void push(const T& value) {
    array_[top_] = value;  // 2. copy assignment operator
    ++top_;
  }
  T pop() {
    --top_;
    return array_[top_];  // 3. copy constructor
  }
  ~Stack() noexcept {
    delete[] array_;  // 4. no-throwing destructor
  }
private:
  size_t size_{0};
  T* array_{nullptr};
  size_t top_{0};
};  // class Stack

}  // namespace bad_case

namespace solution {
/*
T 的要求:
1. 拷贝赋值运算符
2. 不抛异常的析构函数
*/
template <class T, class V>
void construct(T* placement, const V& value) {
  std::println("placement new for T");
  new (placement) T(value);
}
template <class T>
void destruct(const T& t) noexcept {
  std::println("call destructor of T");
  t.~T();
}

template <class T>
class Stack {
public:
  Stack(size_t size): size_(size), array_(static_cast<T*>(::operator new (sizeof(T) * size))) {}
  void push(const T& value) {
    construct(array_ + top_, value);  // 1. 拷贝赋值运算符
    ++top_;
  }
  T top() {
    return array_[top_ - 1];
  }
  void pop() {
    destruct(array_[--top_]);
  }
  ~Stack() {
    std::for_each(array_, array_ + top_, destruct<T>);  // 2. 不抛异常的析构函数
    std::println("operator delete");
    ::operator delete(array_);
  }
private:
  size_t size_{0};
  T* array_{nullptr};
  size_t top_{0};
};  // class Stack
}  // namespace solution

void run() {
  {
    // // no: X no default constructor
    // bad_case::Stack<X> stack(10);
    // X x(1);
    // stack.push(x);
    // stack.push(x);
    // // yes
    // stack.pop();
    // // no: copy assignment operator of X is private
    // auto top = stack.pop();
  }
  {
    solution::Stack<X> stack(10);  // operator new
    X x(1);
    stack.push(x);  // placement new
    auto top = stack.top();  // copy constructor
    stack.pop();  // operator delete
  }
}

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}

