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
*/

#include <cassert>
#include <print>
#include <stdexcept>

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

class A {
    B b_;
    C c_;
    D d_;
    enum class TrackerType { NONE, ONE, TWO, THREE };
public:
    A(TrackerType tracker = TrackerType::NONE)
    try:
        b_((tracker = TrackerType::ONE, "HELLO")),
        c_((tracker = TrackerType::TWO, "world")),
        d_((tracker = TrackerType::THREE, GetD())) {
            assert(tracker == TrackerType::TWO);
    } catch (const std::exception& e) {
        if (tracker == TrackerType::ONE) {
            std::println("B threw: {}", e.what());
        } else if (tracker == TrackerType::TWO) {
            std::println("C threw: {}", e.what());
        } else if (tracker == TrackerType::THREE) {
            std::println("D threw: {}", e.what());
        }
        throw;
    }
};

int main() {
    try {
        A a;
    } catch (const std::exception& e) {
        std::println("Caught: {}", e.what());
    }
    return 0;
}

