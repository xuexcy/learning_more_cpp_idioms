/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/04 15:51:47
# Desc    :
########################################################################

在一些不支持 NRVO 的编译器上，通过将实例的计算逻辑放到构造函数，然后通过使用 RVO 进行替代
也就是不用 {
    Object obj;
    xxx;
    xxx;
    return obj;
}, 而是使用 {
    return Object(xxx);
}

rvo: return value optimization 返回值优化，函数返回一个新构造的临时对象(未命名)，比如 return Data(c)
Nrvo: named return value optimization 命名返回值优化，函数返回一个已命名对象，比如
    Data c;
    c.set_data('c');
    return c;
*/

#include <algorithm>
#include <new>
#include <print>

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

struct Data {
    Data(char c = 0) {
        std::println("{}", CURRENT_FUNCTION_NAME);
        std::fill(bytes, bytes + 16, c);
    }
    Data(const Data& d) {
        std::copy(d.bytes, d.bytes + 16, this->bytes);
        std::println("A copy was made.\n");
    }
    void print() {
        std::println("{}", bytes);
    }
    ~Data() {
        std::println("{}", CURRENT_FUNCTION_NAME);
    }
private:
    char bytes[16];
};

namespace bad_case_rvo {
Data run_copy_and_assign(char c) {
    return Data(c);
}

void run() {
    std::println("Start bad_case_rvo");
    // 这里编译器会优化，最终在 run_copy_and_assign 函数里调用一次构造函数，然后在 run 函数结束后调用一次 d 的析构函数
    // 如果没有 rvo，那么在调用 run_copy_and_assign 时，会传入一个临时变量的地址，然后
    //  1. 在 run_copy_and_assign 中构造 Data(c)
    //  2. 在 run_copy_and_assign 中 return 时，使用 Data(c) 拷贝构造到这个临时变量
    //  3. 在此处 auto d = xxx; 时将临时变量赋值(拷贝)构造到 d
    // 于是发生了 1次普通构造+2次拷贝构造，最后再发生三次析构
    auto d = run_copy_and_assign('c');
    d.print();
    std::println("End bad_case_rvo");
}
}  // namespace bad_case_rvo

namespace solution_rvo {
// 在没有 rvo 的情况下，可以使用 placement new 来实现 rvo，但是最后要自己主动调用析构函数
void run_rvo(Data* target, char c) {
    new (target) Data(c);
    return;
}
void run() {
    std::println("Start solution_rvo");
    char bytes[sizeof(Data)];
    auto target = reinterpret_cast<Data*>(bytes);
    run_rvo(target, 'r');
    target->print();
    target->~Data();
    std::println("End solution_rvo");
}
}  // namespace solution_rvo

namespace bad_case_nrvo {
class File {
private:
    std::string str_;
public:
    File() {}
    void path(std::string_view path) {
        str_ += path;
    }
    void name(std::string_view name) {
        str_ += '/';
        str_ += name;
    }
    void ext(std::string_view ext) {
        str_ += '.';
        str_ += ext;
    }
    void print() {
        std::println("{}", str_);
    }
};

File getfile() {
    File f;
    f.path("/lib");
    f.name("libc");
    f.ext("so");
    f.ext("6");
    // no RVO because object has a name 'f'
    // not all compiler support NRVO
    return f;
}

void run() {
    std::println("Start bad_case_nrvo");
    auto f = getfile();
    f.print();
    std::println("End bad_case_nrvo");
}

} // namespace bad_case_nrvo

namespace solution_nrvo {
// 通过将计算逻辑放到构造函数，避免使用一个 named variable
// 这样可以通过编译器的 rvo 来做到 nrvo
class File {
private:
    std::string str_;
public:
    File() {}
    File(
        const std::string& path, const std::string& name,
        const std::string& ext1, const std::string& ext2)
    :str_(path + "/" + name + "." + ext1 + "." + ext2) {
    }
    void print() {
        std::println("{}", str_);
    }
};

File getfile() {
    return File("/lib",  "libc", "so", "6");
}

void run() {
    std::println("Start solution_nrvo");
    auto f = getfile();
    f.print();
    std::println("End solution_nrvo");
}
}  // namespace solution_nrvo

int main() {
    utils::MainDecorator::Access();
    bad_case_rvo::run();
    solution_rvo::run();

    bad_case_nrvo::run();
    solution_nrvo::run();
    return 0;
}

