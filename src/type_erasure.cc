/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/02/26 23:26:58
# Desc   :
########################################################################
类型擦除：让一个类中可以存储多个不同的类型，比如 std::variant<int, double>
*/

#include <memory>
#include "utils/main_decorator.h"


/**
实现方法: 在 var 中存储一个基类指针，该指针指向继承类，并且这个继承类是一个模板类，这样这个继承类就可以存储任意类型
1. inner_base: 定义一个基类，声明一个纯虚函数 Type() 方法，返回类型信息
2. template <class T> struct inner: 继承 inner_base, 存储一个类型为 T 的数据，并实现 Type() 方法
3. struct var: 存储 inner_base 指针，并提供 operator= 方法，这样，该指针就可以指向任意的 inner<T> 对象
*/
struct var {
  struct inner_base{
    using ptr = std::unique_ptr<inner_base>;
    virtual const std::type_info& Type() const = 0;
  };  // struct inner_base
  template <class T>
  struct inner : inner_base {
    inner(T newval): value_(newval) {}
    virtual const std::type_info& Type() const override {
      return typeid(T);
    }
  private:
    T value_;
  };  // struct inner : inner_base


  template <class T>
  var(T src): inner_(new inner<T>(std::forward<T>(src))) {}
  template <class T>
  var& operator=(T src) {
    inner_ = std::make_unique<inner<T>>(std::forward<T>(src));
    return *this;
  }
private:
  typename inner_base::ptr inner_;
};  // struct var



namespace type_erasure {
struct var {
  // 构造函数
  var(): inner_(new inner<int>(0)) {}
  var(const var& src): inner_(src.inner_->clone()) {}
  template <class T>
  var(T src): inner_(new inner<T>(std::forward<T>(src))) {}

  // 赋值操作符
  template <class T>
  var& operator=(T src) {
    inner_ = std::make_unique<inner<T>>(std::forward<T>(src));
    return *this;
  }
  var& operator=(const var& src) {
    var tmp(src);
    std::swap(tmp.inner_, this->inner_);;
    return *this;
  }

  const std::type_info& Type() const { return inner_->Type(); }

  // 直接进行 dynamic_cast
  template <class T>
  T& cast() {
    return *dynamic_cast<inner<T>*>(*inner_);
  }
  template <class T>
  const T& cast() const {
    return *dynamic_cast<inner<T>*>(*inner_);
  }

  struct inner_base {
    using Pointer = std::unique_ptr<inner_base>;
    virtual ~inner_base() = default;
    virtual inner_base* clone() const = 0;
    virtual const std::type_info& Type() const = 0;
  };  // struct inner_base
  template <class T>
  struct inner : inner_base {
    inner(T newval): value_(std::move(newval)) {}
    virtual inner_base* clone() const override {
      return new inner(value_);
    }
    virtual const std::type_info& Type() const override {
      return typeid(T);
    }
    T& operator* () { return value_; }
    const T& operator* () const { return value_; }
  private:
    T value_;
  };  // struct inner
private:
  inner_base::Pointer inner_;
};  // struct var
}  // namespace type_erasure

int main() {
  utils::MainDecorator::Access();
  return 0;
}
