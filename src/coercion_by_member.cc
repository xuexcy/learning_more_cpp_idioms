/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/10/31 22:55:33
# Desc    :
########################################################################
*/

/**
Derived 类的指针可以转换成 Base 类的指针，但是如果是封装后的指针，则不能直接转换, 如下:
yes:
    D* -> B*
no:
    bad_case::Helper<D> -> bad_case::Helper<B>
    bad_case::Helper<D*> -> bad_case::Helper<B*>
    std::unique_ptr<D> -> std::unique_ptr<B>
解决办法如下
1. 同类型转换 T -> T : 实现非模板拷贝构造和拷贝赋值函数
2. 不同类型转换 U-> T : 实现模板拷贝构造和拷贝赋值函数

注意:
1. 如果不主动实现同类型转换，即非模板拷贝构造和拷贝赋值函数，那么编译器会生成默认的实现，默认实现将不符合预期
2. 如果有必要，base 类的析构函数需要设置成 virtual，以免 derived 中的资源没有被释放
*/

#include <print>

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

namespace bad_case {

class B {};
class D: public B {};

template <class T>
class Helper {};

void run() {
    // yes, derived ptr to base ptr
    B* bptr;
    D* dptr;
    bptr = dptr;

    // no
    Helper<B> hb;
    Helper<D> hd;
    // hb = hd;

    // no
    Helper<B*> hbptr;
    Helper<D*> hdptr;
    // hbptr = hdptr;

    // no
    std::unique_ptr<B> b_unique_ptr;
    std::unique_ptr<D> d_unique_ptr;
    // d_unique_ptr = b_unique_ptr;
}

}  // namespace bad_case


namespace solution {

class B {
public:
    virtual ~B() {
        // 析构函数需要是虚函数，否则 D 转换成 B 后再 delete 不会调用 D 的析构函数
        std::println("{}", CURRENT_FUNCTION_NAME);
    }
};

class D: public B {
public:
    virtual ~D() {
        std::println("{}", CURRENT_FUNCTION_NAME);
    }
};

template <class T>
class Ptr {
public:
    Ptr() {}
    ~Ptr() {
        if (ptr) {
            delete ptr;
        }
    }
    Ptr(const Ptr& p): ptr(p.ptr) {
        std::println("copy constructor");
    }
    template <class U>
    Ptr(const Ptr<U>& p): ptr(p.ptr) {
        std::println("coercing member template constructor");
    }
    Ptr& operator=(const Ptr& p) {
        ptr = p.ptr;
        std::println("copy assignment operator");
        return *this;
    }
    template <class U>
    Ptr& operator=(const Ptr<U>& p) {
        ptr = p.ptr;
        std::println("coercing member template assignment operator");
        return *this;
    }
    T* ptr;
};

// template <class T, size_t SIZE>
// class Array {
// public:
//     Array() {}
//     Array(const Array& a) {
//         std::copy(a.array_, a.array_ + SIZE, array_);
//     }
//     template <class U>
//     Array(const Array<U*>& a) {
//         std::copy(a.array_, a.array_ + SIZE, array_);
//     }
//     template <class U>
//     Array& operator=(const Array<U*>& a) {
//         std::copy(a.array_, a.array_ + SIZE, array_);
//     }
//     size_t size{SIZE};
//     T array_[SIZE];
// };
void run() {
    Ptr<D> d_ptr;
    d_ptr.ptr = new D;
    Ptr<B> b_ptr(d_ptr);
    b_ptr = d_ptr;
}

}  // namespace solution

int main() {
    bad_case::run();
    solution::run();
}














