/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/05 15:40:07
# Desc    :
########################################################################

在构造函数的初始化列表初始化数据成员时，可能会抛出异常(不同的成员可能抛出相同的异常类型),我们需要确定是哪个数据抛出了异常

@solution: 在列表初始化成员时，使用双括号对 tracker 进行赋值然后返回初始化成员的参数
*/

#include <cassert>
#include <print>
#include <stdexcept>

#include "utils/main_decorator.h"

/*
1. 双括号
2. B、C 的构造必须要有一个参数，否者需要一个 Adapter 类
*/
struct B {
    B(const std::string&) {
       // throw std::runtime_error("B Error");
    }
};

struct C {
    C(const std::string&) {
       throw std::runtime_error("C Error");
    }
};

struct D {
    D() {
        throw std::runtime_error("D Error");
    }
};

D GetD() {
    return D();
}

// 见 parameterized_base.cc，为其他可无参构造的类型 T 赋予带参构造的能力
template <class T>
class Adapter : public T {
public:
    Adapter(int dummy): T() {}
};

struct E {
    E() {
        throw std::runtime_error("E Error");
    }
};

struct F {
    F(const std::string& s1, const std::string& s2) {
        throw std::runtime_error("F Error");
    }
};

/**
关于 b_((tracker = TrackerType::ONE, "HELLO")) 的解释:
    1. tracker = TrackerType::ONE 这个表达式会首先执行，将 tracker 赋值为 TrackerType::ONE
    2. 逗号运算符在 C++ 中的特性是：它会返回它的最后一个操作数的值。因此，"HELLO" 是这个逗号表达式的结果。
双括号 (( )): 在这种情况下主要是表示一个单一的参数，符合 C++ 的函数或构造函数的参数传递语法。
逗号运算符: 返回最后一个表达式的结果，所以赋值操作被使用来改变 tracker 的值，而不是直接返回。
*/

class A {
    B b_;
    C c_;
    D d_;
    Adapter<E> e_;
    F f_;
    enum class TrackerType { NONE, ONE, TWO, THREE, FOUR, FIVE};
public:
    A(TrackerType tracker = TrackerType::NONE)
    try:
        b_((tracker = TrackerType::ONE, "HELLO")),
        c_((tracker = TrackerType::TWO, "world")),
        d_((tracker = TrackerType::THREE, GetD())),
        e_((tracker = TrackerType::FOUR, 1)),
        f_((tracker = TrackerType::FIVE, "hi"), "he") {
            assert(tracker == TrackerType::TWO);
    } catch (const std::exception& e) {
        if (tracker == TrackerType::ONE) {
            std::println("B threw: {}", e.what());
        } else if (tracker == TrackerType::TWO) {
            std::println("C threw: {}", e.what());
        } else if (tracker == TrackerType::THREE) {
            std::println("D threw: {}", e.what());
        } else if (tracker == TrackerType::FOUR) {
            std::println("E threw: {}", e.what());
        } else if (tracker == TrackerType::FIVE) {
            std::println("E threw: {}", e.what());
        }
        throw;
    }
};

/**
@stdout:
C threw: C Error
Caught: C Error
*/
int main() {
    utils::MainDecorator::Access();
    try {
        A a;
    } catch (const std::exception& e) {
        std::println("Caught: {}", e.what());
    }
    return 0;
}

