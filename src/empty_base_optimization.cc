/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/18 14:26:08
# Desc    :
########################################################################
空基类优化
空类的 size 不能为 0，一般为 1，如果某个类有个成员是空类，浪费的空间可能大于 1（编译器要做数据对齐），
如下的 class ContainEmpty 和 class ContainEmptyV2

@solution: c++ 允许在继承空类时，将这个空类的 size 1 优化掉，如下 class DerivedEmpty
*/
#include <cassert>
#include <print>

#include "utils/main_decorator.h"
class Empty1 {
public:
    void print() {
        std::println("I am a instance of class Empty1 instance");
    }
};
class Empty2 {
public:
    void print() {
        std::println("I am a instance of class Empty2 instance");
    }
};
class ContainEmpty {
    int a{0};
    Empty1 e1;
    Empty2 e2;
};
class ContainEmptyV2 {
    Empty1 e1;
    int a{0};
    Empty2 e2;
};
class DerivedEmpty : private Empty1, private Empty2 {
    int a{0};
};
// 当用户使用 DerivedEmpty 时，是可以看到 Empty1 和 Empty2 的函数签名的(虽然使用了 private 继承，无法调用基类的函数)
// 如下的在 DerivedEmptyV2 中是看不到 Empty1 和 Empty2 的签名的（虽然我也不知道这有啥子用）
template <class Base1, class Base2, class Member>
struct BaseOptimization : Base1, Base2 {
    Member member;
    BaseOptimization() = default;
    BaseOptimization(const Base1& b1, const Base2& b2, const Member& mem):
        Base1(b1), Base2(b2), member(mem) {}
};
class DerivedEmptyV2 {
    BaseOptimization<Empty1, Empty2, int> data;
};

void run() {
    std::println("sizeof({}): {}", "Empty1", sizeof(Empty1));
    assert(1 == sizeof(Empty1));
    std::println("sizeof({}): {}", "Empty2", sizeof(Empty2));
    assert(1 == sizeof(Empty2));
    std::println("sizeof({}): {}", "ContainEmpty", sizeof(ContainEmpty));
    assert(8 == sizeof(ContainEmpty));
    std::println("sizeof({}): {}", "ContainEmptyV2", sizeof(ContainEmptyV2));
    assert(12 == sizeof(ContainEmptyV2));
    std::println("sizeof({}): {}", "DerivedEmpty", sizeof(DerivedEmpty));
    assert(4 == sizeof(DerivedEmpty));  // 空基类优化
    std::println("sizeof({}): {}", "DerivedEmptyV2", sizeof(DerivedEmptyV2));
    assert(4 == sizeof(DerivedEmptyV2));  // 空基类优化
}


int main() {
    utils::MainDecorator::Access();
    run();
    return 0;
}

