/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/06 14:14:05
# Desc    :
########################################################################

使用 copy-and-swap 来保证赋值操作时 strong exception safety

异常安全的三个 level:
    - basic guarantee(基本保证): 还能用。如果抛出异常，程序内的任何事物仍保持有效状态
    - strong guarantee(强烈保证): 像没发生一样。如果抛出异常，程序状态不变。如果函数调用成功，就完成成功，如果失败，就会回到调用前的状态(相当于会滚)
    - no-throw(不抛异常): 承诺绝不抛出异常。它们总是能够完成它们预先承诺的功能
*/

#include <cassert>
#include <print>
#include <random>

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

class BaseString {
private:
    char* str_{nullptr};
public:
    BaseString() {
        std::println("默认构造");
        PRINT_CURRENT_FUNCTION_NAME;
    }
    BaseString(const char* s) {
        std::println("普通构造");
        PRINT_CURRENT_FUNCTION_NAME;
        copy(s);
        print_internal();
    }
    BaseString(const BaseString& s) {
        std::println("拷贝构造");
        PRINT_CURRENT_FUNCTION_NAME;
        copy(s.str_);
        print_internal();
    }
    BaseString(BaseString&& s) {
        std::println("移动构造");
        PRINT_CURRENT_FUNCTION_NAME;
        swap(s);
        print_internal();
    }
    virtual ~BaseString() {
        PRINT_CURRENT_FUNCTION_NAME;
        print_internal();
        release();
    }
    void swap(BaseString& s) noexcept {
        // non-throwing swap
        std::swap(this->str_, s.str_);
    }
    void print() {
        PRINT_CURRENT_FUNCTION_NAME;
        print_internal();
    }
private:
    void copy(const char* s) {
        release();
        str_ = new char[strlen(s) + 1];
        strcpy(str_, s);
    }
    void print_internal() {
        if (nullptr == str_) {
            std::println("    this = {}, &str = {}", (void*)this, "nullptr");
        } else {
            std::println("    this = {}, &str = {}, str = {}", (void*)this, (void*)str_, str_);
        }
    }
    void release() {
        if (nullptr != str_) {
            delete [] str_;
            str_ = nullptr;
        }
    }
};

namespace solution_1 {

std::random_device rd;
std::mt19937 gen(rd());
const int MAX_N{2};
std::uniform_int_distribution<> dis(0, MAX_N);

class String : public BaseString {
public:
    String() = default;
    String(const char* s): BaseString(s) {
    }
    String(const String& s): BaseString(s) {
    }
    String(String&& s): BaseString(s){
    }
    // 拷贝赋值运算符用于将一个对象的值赋给已经存在的对象
    String& operator=(const String& s) {
        std::println("拷贝赋值");
        PRINT_CURRENT_FUNCTION_NAME;
        if (&s == this) {  // self-assignment cases
            return *this;
        }
        // 拷贝构造
        if (dis(gen) == 0) {  // implementation style 1
            String temp(s);  // copy constructor
            temp.swap(*this);
        }  // release temp
        else { // implementation style 2
            String(s).swap(*this);
        } // release String(s)

        return *this;
    }
};


}  // namespace solution_1

namespace solution_2 {

class String : public BaseString {
public:
    String() = default;
    String(const char* s): BaseString(s) {
    }
    String(const String& s): BaseString(s) {
    }
    // 统一赋值运算符 unifying assignment operator
    // 通过一个 operator= 来实现 copy assignment 和 move assignment
    String& operator=(String s) {
        std::println("拷贝赋值");
        PRINT_CURRENT_FUNCTION_NAME;
        s.swap(*this);
        return *this;
    }  // release s
};

}  // namespace solution_2

template <class T>
void run(const char* value) {
    // 普通构造
    T s1(value);
    s1.print();
    std::println("&s1 = {}\n", (void*)&s1);

    // 拷贝构造
    auto s2 = s1;
    s2.print();
    std::println("&s2 = {}\n", (void*)&s2);

    // 默认构造
    T s3;
    s3.print();
    // 拷贝赋值运算符用于将一个对象的值赋给已经存在的对象
    // 拷贝赋值
    s3 = s1;
    s3.print();
    std::println("&s3 = {}\n", (void*)&s3);

    // 拷贝构造
    T s4 = std::move(s1);
    s4.print();
    std::println("&s4 = {}\n", (void*)&s4);

    // 默认构造
    T s5;
    s5.print();
    // 拷贝赋值
    s5 = std::move(s1);
    s5.print();
    std::println("&s5 = {}\n", (void*)&s5);
}

template <class T>
T GetT(const char* value) {
    T v(value);
    return v;
}
template <class T>
void copy_assignment(const char* value) {
    std::println("\nStart {}", value);
    T t;
    t.print();
    std::println("&t = {}\n", (void*)&t);

    t = GetT<T>(value);
    t.print();
    std::println("&t = {}\n", (void*)&t);
    std::println("End {}", value);
}

/**
solution_1 和 solution_2 的不同之处在于拷贝赋值的参数和内部实现方式:
    solution_1: const reference(pass by reference) + 内部临时变量 temp (内部临时变量会发生一次拷贝构造)
    solution_2: pass by value。pass by value 会理论上应该发生一次拷贝构造)但在下面的情形可能会出现 copy elision
在 copy_assignment 函数的 t = Get<T>(value) 这里:
    1. 在函数里面发生了一次普通构造: T v(value) ，这一点两个 solution 没有区别
    2. 在将返回值 v 赋值给 t 时，发生了拷贝赋值, 两种 solution 的 difference 如下:
        a. solution_1 的拷贝赋值内部通过对 v (pass by reference) 拷贝构造产生了一个临时变量 temp
        b. solution_2 的拷贝赋值直接使用了返回值 v 的 pass by value 的参数，本来 v 的 pass_by_value
            也应该产生一个拷贝构造用作拷贝赋值的实参，但是由于函数的返回值 v 在拷贝构造完后就不需要了，
            于是发生了 copy elision(编译期支持)，即直接将 v 作为 solution_2 的拷贝赋值的实参进行使用，
            之后再析构掉函数返回值 v
        c. solution_1 和 solution_2, 都应该发生一次拷贝构造，只是这个拷贝构造是发生在拷贝赋值函数内的临时变量,
            还是发生在拷贝赋值函数传参时的 pass by value，但是对于将其他函数的返回值 pass by value 给到 solution_2
            的拷贝赋值函数做实参时，发生了 copy elision，于是这种情况下 solution_2 比 solution_1 少了一次拷贝构造

@stdout: solution_1_copy_assignment
Start solution_1_copy_assignment
默认构造
BaseString::BaseString()
void BaseString::print()
    this = 0x7ff7b71ca698, &str = nullptr
&t = 0x7ff7b71ca698

普通构造
BaseString::BaseString(const char *)
    this = 0x7ff7b71ca660, &str = 0x7fe3f59040c0, str = solution_1_copy_assignment
拷贝赋值
String &solution_1::String::operator=(const String &)
拷贝构造
BaseString::BaseString(const BaseString &)
    this = 0x7ff7b71ca5c8, &str = 0x7fe3f5904080, str = solution_1_copy_assignment
virtual BaseString::~BaseString()
    this = 0x7ff7b71ca5c8, &str = nullptr
virtual BaseString::~BaseString()
    this = 0x7ff7b71ca660, &str = 0x7fe3f59040c0, str = solution_1_copy_assignment
void BaseString::print()
    this = 0x7ff7b71ca698, &str = 0x7fe3f5904080, str = solution_1_copy_assignment
&t = 0x7ff7b71ca698

End solution_1_copy_assignment
virtual BaseString::~BaseString()
    this = 0x7ff7b71ca698, &str = 0x7fe3f5904080, str = solution_1_copy_assignment

可以看到下面的输出比上面上了一次拷贝构成
@stdout: solution_2_copy_assignment
Start solution_2_copy_assignment
默认构造
BaseString::BaseString()
void BaseString::print()
    this = 0x7ff7b71ca698, &str = nullptr
&t = 0x7ff7b71ca698

普通构造
BaseString::BaseString(const char *)
    this = 0x7ff7b71ca660, &str = 0x7fe3f5904080, str = solution_2_copy_assignment
拷贝赋值
String &solution_2::String::operator=(String)
virtual BaseString::~BaseString()
    this = 0x7ff7b71ca660, &str = nullptr
void BaseString::print()
    this = 0x7ff7b71ca698, &str = 0x7fe3f5904080, str = solution_2_copy_assignment
&t = 0x7ff7b71ca698

End solution_2_copy_assignment
virtual BaseString::~BaseString()
    this = 0x7ff7b71ca698, &str = 0x7fe3f5904080, str = solution_2_copy_assignment
*/
void solution_difference() {
    PRINT_CURRENT_FUNCTION_NAME;
    copy_assignment<solution_1::String>("solution_1_copy_assignment");
    copy_assignment<solution_2::String>("solution_2_copy_assignment");
}

int main() {
    utils::MainDecorator::Access();
    run<solution_1::String>("solution_1");
    run<solution_2::String>("solution_2");

    std::println();
    solution_difference();
    return 0;
}

