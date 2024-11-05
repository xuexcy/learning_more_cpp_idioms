/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/04 23:13:48
# Desc    :
########################################################################

确保对象在使用前进行了初始化(完成构造)，特别是非局部静态对象

如 bad_case::Foo::bar_ 所示，作为一个 class 中的 static object，其构造在 foo 之后，但其使用在 foo 构造时
(bar_ 还没有完成构造就使用了)，这样可能造成不符合预期的结果

分析：bad_case 中直接使用了 bar_，但没法保证 bar_ 已经完成了构造，那我们的解决办法可以是不直接使用 bar_，而是通过
函数获取 bar_，然后在函数中保证 bar_ 的构造

@solution: 将 class static variable 放到 class static/member function 中，在 function 中确保 variable 在
使用前完成构造。
*/

#include <print>

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

struct Bar {
    Bar(const std::string& name): name_(name) {
        PRINT_CURRENT_FUNCTION_NAME;
        std::println("Bar::name_: {}", name_);
    }
    void f() {
        PRINT_CURRENT_FUNCTION_NAME;
    }
    ~Bar() {
        PRINT_CURRENT_FUNCTION_NAME;
        std::println("Bar::name_: {}", name_);
    }
private:
    std::string name_;
};

namespace bad_case {
/**
如下输出： foo 在构造时调用了 bar_.f()(但此时 bar_ 还没构造)，构造完 foo 后才构造 Foo::bar_
@stdout_before_main:
bad_case::Foo::Foo()
void Bar::f()
Bar::Bar(const std::string &)
Bar::name_: bad_case
@stdout_after_main:
Bar::~Bar()
Bar::name_: bad_case
 */
struct Foo {
    Foo() {
        PRINT_CURRENT_FUNCTION_NAME;
        bar_.f();
    }
    static Bar bar_;
};

Foo foo;
Bar Foo::bar_("bad_case");

}  // namespace bad_case

namespace solution_1 {
/**
在 Foo::Bar& bar() 里 new 了一个 static Bar* b; 但是程序结束时没有进行 delete (可以看到@stdout_after_main
并没调用 Bar 的析构函数的输出，在 solution_2 是有析构的)，如果 Bar 的析构是 trivial 的，那就无所谓，因为程序在结束
时会回收 new 申请的内存，也就不会造成内存泄漏(没问题但不太好，还是要主动把 new 的内存 delete 掉)，但如果析构是
none-trivial 的，那就需要主动析构一下，不管哪种情况，都建议用 solution_2 中的 local static object,而不是这里的
dynamic allocation

@stdout_before_main:
solution_1::Foo::Foo()
Bar::Bar(const std::string &)
Bar::name_: solution_1
void Bar::f()
@stdout_after_main:
 */

struct Foo {
    Foo() {
        PRINT_CURRENT_FUNCTION_NAME;
        bar().f();
    }
    Bar& bar() {
        static Bar* b = new Bar("solution_1");
        return *b;
    }
};

Foo foo;
}  // namespace solution_1

namespace solution_2 {
/**
@stdout_before_main:
solution_2::Foo::Foo()
Bar::Bar(const std::string &)
Bar::name_: solution_2
void Bar::f()
@stdout_after_main:
Bar::~Bar()
Bar::name_: solution_2
 */
struct Foo {
    Foo() {
        PRINT_CURRENT_FUNCTION_NAME;
        Foo::bar().f();
    }
    // 在 solution_1 中，这里不是 static Bar，不过无所谓，两种都可以
    static Bar& bar() {
        static Bar b("solution_2");
        return b;
    }
};
Foo foo;
}  // namespace solution_2

int main() {
    utils::MainDecorator::Access();
    return 0;
}

