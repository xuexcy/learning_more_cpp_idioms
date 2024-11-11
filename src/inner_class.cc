/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/11 08:39:08
# Desc    :
########################################################################

问题: Derived 类在继承多个不同 Base 类时，如果 Base 类之间有同名的虚函数，那么 Derived 在继承 Base 后无法实现不同 Base
的虚函数
@solution: 不要由 Derived 去继承 Base，而是通过在 Derived 中定义多个 Inner class 分别去继承各个 Base，并由这些 Inner
去实现 Base 中的虚函数，另外由 Derived 提供接口去调用这些 Base 的虚函数

*/

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

class Base1 {
public:
    void haha() { std::println("haha, Base1"); }
    virtual int open(int) = 0;
    virtual ~Base1() {}
};

class Base2 {
public:
    void haha() { std::println("haha, Base2"); }
    virtual int open(int) = 0;
    virtual ~Base2() {}
};

namespace bad_case {
// idiom: interface_class
class Derived : public Base1, public Base2 {
public:
    // Base1 和 Base2 都有 virtual int open(int)
    // 下面这个编译和运行都没有问题，到底是 override 的哪个基类的呢？
    // 以及，我想要为 Base1 和 Base2 各实现一个的 open 怎么办？
    int open(int i) override {
        PRINT_CURRENT_FUNCTION_NAME;
        return i;
    }
    ~Derived() {}
};

void run() {
    std::println("start bad_case");
    Derived d;
    std::println("{}", d.open(2));
    // Member 'haha' found in multiple base classes of different types
    // d.haha();
    static_cast<Base1*>(&d)->haha();
    static_cast<Base2*>(&d)->haha();
    std::println("end bad_case\n");
}

}  // bad_case


namespace solution {
/**
通过 class BaseXXX_Impl : public BaseXXX 来实现各个 Base 的虚函数，并由 Derived 提供接口 base1_open 和 base2_open
来分别调用基类中的 open
 */

class Derived {
    class Base1_Impl;
    friend class Base1_Impl;
    class Base1_Impl : public Base1 {
    public:
        Base1_Impl(Derived* p): parent_(p) {}
        int open(int i) override {
            PRINT_CURRENT_FUNCTION_NAME;
            auto res = i * 2;
            std::println("i: {}, i * 2: {}", i, res);
            return res;
        }
    private:
        Derived* parent_{nullptr};
    };
    class Base2_Impl;
    friend class Base2_Impl;
    class Base2_Impl : public Base2 {
    public:
        Base2_Impl(Derived* p): parent_(p) {}
        int open(int i) {
            PRINT_CURRENT_FUNCTION_NAME;
            auto res = i * i;
            std::println("i: {}, i * i: {}", i, res);
            return res;
        }
    private:
        Derived* parent_{nullptr};
    };
public:
    Derived(): base1_obj_(this), base2_obj_(this) {}
    Derived(const Derived&): base1_obj_(this), base2_obj_(this) {}
    Derived(Derived&&): base1_obj_(this), base2_obj_(this) {}

    Derived& operator=(const Derived&) { return *this; }
    Derived& operator=(Derived&&) { return *this; }
public:
    int base1_open(int i) {
        PRINT_CURRENT_FUNCTION_NAME;
        return base1_obj_.open(i);
    }
    int base2_open(int i) {
        PRINT_CURRENT_FUNCTION_NAME;
        return base2_obj_.open(i);
    }
    // convert derived to base
    operator Base1&() { return base1_obj_; }
    operator Base2&() { return base2_obj_; }
private:
    Base1_Impl base1_obj_;
    Base2_Impl base2_obj_;
};

int base1_open(Base1& b1, int i) {
    PRINT_CURRENT_FUNCTION_NAME;
    return b1.open(i);
}

int base2_open(Base2& b2, int i) {
    PRINT_CURRENT_FUNCTION_NAME;
    return b2.open(i);
}

void run() {
    std::println("start solution");
    Derived d;
    std::println("{}", d.base1_open(3));
    std::println("{}", d.base2_open(3));
    std::println("{}", base1_open(d, 3));
    std::println("{}", base2_open(d, 3));
    std::println("end solution\n");
}

}  // namespace solution

int main() {
    utils::MainDecorator::Access();
    bad_case::run();
    solution::run();
    return 0;
}

