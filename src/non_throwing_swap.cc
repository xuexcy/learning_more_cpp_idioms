/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/08 13:52:01
# Desc    :
########################################################################

实现异常安全(safe)和高效(efficient)的 swap 操作
exception safe swap

下面的 bad_case 中 temp 的问题是
1. 如果 T 是一个很大、很复杂的类型，那么构造和析构临时变量 temp 不是一个高效操作
2. 如果内存资源不够，在为 temp 申请资源时就会抛出异常
也就是说我们不应该新建一个临时变量，自然不新建变量就不会申请内存，也就不会抛出异常

下面的实现方式用的一种叫 handle body 的方法，即外部 class 是 handle，内部包含一个 body (即所持数据) 的指针, 通过
swap 指针的方式来交换数据，达到 no throwing 的目的

*/

#include <utility>

#include "cpp_utils/util.h"

namespace bad_case {
template <class T>
void swap(T& a, T& b) {
    T temp(a);  // 这里会使用 a 构造一个 temp ，等函数结束后再析构 temp，如果 T 是一个很复杂的类型，那就太耗费性能了
    a = b;
    b = temp;
}
}  // namespace bad_case


namespace solution {
class String {
    char* str{nullptr};
public:
    void swap(String& s) {
        PRINT_CURRENT_FUNCTION_NAME;
        std::swap(this->str, s.str);
    }
};
// unqualified swap
void swap(String& s1, String& s2) {
    PRINT_CURRENT_FUNCTION_NAME;
    s1.swap(s2);
}
} // namespace solution

// fully qualified swap
template <>
void std::swap(solution::String& s1, solution::String& s2) noexcept {
    PRINT_CURRENT_FUNCTION_NAME;
    s1.swap(s2);
}

namespace solution {
class UserDefined {
    String str;
public:
    void swap(UserDefined& u) {
        PRINT_CURRENT_FUNCTION_NAME;
        std::swap(str, u.str);
    }
};
}  // namespace solution

template <>
void std::swap(solution::UserDefined& u1, solution::UserDefined& u2) noexcept {
    PRINT_CURRENT_FUNCTION_NAME;
    u1.swap(u2);
}

namespace solution {
class MyClass {
    UserDefined u;
    char* name;
public:
    void swap(MyClass& m) {
        PRINT_CURRENT_FUNCTION_NAME;
        std::swap(u, m.u);
        std::swap(name, m.name);
    }
};

}  // namespace solution

template <>
void std::swap(solution::MyClass& m1, solution::MyClass& m2) noexcept {
    PRINT_CURRENT_FUNCTION_NAME;
    m1.swap(m2);
}

// 如果 T 是 solution::String，那么就会使用上面自定义的特化的 std::swap
// 如果不是，那么就会由标准库中的 std::swap 进行特化
template <class T>
void zoo(T t1, T t2) {
    int i1{0};
    int i2{0};
    std::swap(i1, i2);
    solution::swap(t1, t2);
    std::swap(t1, t2);
}

void run() {
    solution::String s1;
    solution::String s2;
    double d1{1.0};
    double d2{2.0};
    zoo(s1, s2);
}


// handle + body
int main() {
    run();
    return 0;
}

