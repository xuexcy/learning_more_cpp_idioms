/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/16 14:34:30
# Desc    :
########################################################################
共享数据指针并计数，计数归 0 时 delete, 也就是 std::shared_ptr 实现的功能
*/

#include <format>
#include <print>

#include "cpp_utils/util.h"
#include "utils/main_decorator.h"

namespace solution {
class StringRep {
    friend class String;
    friend struct std::formatter<StringRep>;
public:
    StringRep(const char* s): count_(1) {
        strcpy(data_ = new char[strlen(s) + 1], s);
    }
    ~StringRep() { delete [] data_; }
private:
    int count_;
    char* data_;
};
}  // namespace solution
template <>
struct std::formatter<solution::StringRep> : std::formatter<std::string> {
    auto format(solution::StringRep& s, format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("[\"{}\", {}]", s.data_, s.count_),
            ctx);
    }
};


namespace solution {
class String {
public:
    String(): rep(new StringRep("")) {
        std::println("empty constructor: {}", *rep);
    }
    String(const String& s): rep(s.rep) {
        ++rep->count_;
        std::println("String constructor: {}", *rep);
    }
    String(const char* s): rep(new StringRep(s)) {
        std::println("char constructor: {}", *rep);
    }
    // 看 copy_and_swap.cc solution_2
    // 这里我们使用 String s 而不是 const String& s
    // String& operator=(const String& s) {
    String& operator=(String s) {
        std::println("before assign: left {} , right {}", *rep, *s.rep);
        s.swap(*this);
        std::println("after assign: left {} , right {}", *rep, *s.rep);
        return *this;
    }
    ~String() {
        if (rep && --rep->count_ <= 0) {
            std::println("destructor: {}", *rep);
            delete rep;
        }
    }
private:
    void swap(String& s) throw() {
        std::swap(this->rep, s.rep);
    }
private:
    StringRep* rep;
};
}  // namespace solution

template <>
struct std::formatter<std::shared_ptr<std::string>> : std::formatter<std::string> {
    auto format(std::shared_ptr<std::string>& s, format_context& ctx) const {
        return std::formatter<std::string>::format(
            std::format("[\"{}\", {}]", *s, s.use_count()),
            ctx);
    }
};

namespace solution_with_shared_ptr {
class String {
public:
    String(): rep(std::make_shared<std::string>("")) {
        std::println("empty constructor: {}", rep);
    }
    String(const String& s): rep(s.rep) {
        std::println("String constructor: {}", rep);
    }
    String(const char* s): rep(std::make_shared<std::string>(s)) {
        std::println("char constructor: {}", rep);
    }
    String& operator=(String s) {
        std::println("before assign: left {} , right {}", rep, s.rep);
        s.swap(*this);
        std::println("after assign: left {} , right {}", rep, s.rep);
        return *this;
    }
    ~String() {
        if (nullptr != rep && rep.use_count() <= 1) {
            std::println("destructor: [{}, {}]", *rep, rep.use_count() - 1);
        }
    }
private:
    void swap(String& s) throw() {
        std::swap(this->rep, s.rep);
    }
private:
    std::shared_ptr<std::string> rep;
};
}  // namespace solution_with_shared_ptr

template <class T>
void run() {
    std::println();
    PRINT_CURRENT_FUNCTION_NAME;
    std::println("*** init String a with empty");
    // String("")  ["", 1]
    T a;
    std::println("\n*** assign a to \"A\"");
    // operator=(&a, String("A")) ["A", 1]
    a = "A";

    std::println("\n*** init String b with \"B\"");
    // String("B") ["B", 1]
    T b = "B";

    std::println("*** init b->a");
    // operator=(&a, String(b)) ["B", 2]
    a = b;

    std::println("\n*** init c with a");
    // String(a) ["B", 3]
    T c(a);

    std::println("\n*** init d with \"D\"");
    // String("D") ["D", 1]
    T d("D");
}

int main() {
    utils::MainDecorator::Access();
    run<solution::String>();
    run<solution_with_shared_ptr::String>();
    return 0;
}

