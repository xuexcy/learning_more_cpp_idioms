/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/19 21:45:16
# Desc    :
########################################################################
SFINAE: Substitution Failure Is Not An Error

模板实例化成功则展开，失败则删除
*/

#include <print>

#include "utils/main_decorator.h"
template <class T>
struct is_pointer {
    // 类型为 U 的指针
    template <class U>
    static char is_ptr(U*);

    // X 的成员且类型为 Y 的指针
    template <class X, class Y>
    static char is_ptr(Y X::*);

    // 不接受任何参数且返回类型 U 的函数指针
    template <class U>
    static char is_ptr(U(*)());

    // 如果不是上述类型，则返回 double
    static double is_ptr(...);

    static T t;
    enum { value = (sizeof(is_ptr(t)) == sizeof(char)) };
};

struct Foo {
    int bar;
};

int main() {
    utils::MainDecorator::Access();
    using IntPtr = int*;
    using FooMemberPtr = int Foo::*;
    using FuncPtr = int(*)();
    // 1
    std::println("{}", int(is_pointer<IntPtr>::value));
    // 1
    std::println("{}", int(is_pointer<FooMemberPtr>::value));
    // 1
    std::println("{}", int(is_pointer<FuncPtr>::value));

    // 0
    std::println("{}", int(is_pointer<char>::value));

    return 0;
}

