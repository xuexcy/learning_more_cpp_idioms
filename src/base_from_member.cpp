/**
使用 derived class 的成员初始化 base class
比如 Derived::Derived(int n): Base(&a_), a_(n) {}
Base 需要 Derived 的成员 a_ 进行初始化，但是构造函数的初始化顺序是先构造 Base 再构造 Derived 的成员

@solution: 将 Base 依赖的成员提取到一个新的 class ContainA，由 Derived 控制继承顺序解决初始化依赖问题
比如
class ContainA() {
public:
    ContainA(int n): a_(n) {}
private:
    int a_{0};
};
class Derived : protected ContainA, public Base {
public:
    Derived(int n): ContainA(n), Base(&a_) {}
};

如上即可通过控制 Derived 继承基类的顺序来控制各个依赖的初始化顺序

*/

#include <cassert>
#include <print>

#include "utils/main_decorator.h"

template <class T>
std::pair<void* /*start*/, void* /*end*/> address(T* obj) {
    // T* end = start + 1;
    return std::make_pair((void*)obj, (void*)(obj + 1));
}

template <class C, class V>
void print_obj(std::string_view obj_name, C* obj, V obj_value) {
    auto [start, end] = address(obj);
    std::println("addressof {}: start {} end {}, value: {}",
        obj_name, start, end, obj_value);
}


class BaseA {
public:
    BaseA(int a): a_(a) {
        std::println("Start BaseA constructor, sizeof: {}", sizeof(*this));
        print_obj("BaseA", this, value());
        print_obj("BaseA::a_", &a_, a_);
        std::println("End BaseA constructor\n");
    }
    int value() const { return a_; }
private:
    int a_{0};
};
class BaseB {
public:
    auto value() { return b_; }
    BaseB(BaseA* obj_a): b_(obj_a->value()) {
        std::println("Start BaseB constructor, sizeof: {}", sizeof(*this));
        print_obj("BaseA", obj_a, obj_a->value());
        print_obj("BaseB", this, value());
        print_obj("BaseB::b_", &b_, b_);
        std::println("End BaseB constructor\n");
    }
private:
    // std::invoke_result_t<decltype(&BaseA::value), BaseA> b_;
    decltype(std::declval<BaseA>().value()) b_;
};


class DerivedA : public BaseA {
public:
    explicit DerivedA(int n): BaseA(n) {
        std::println("Start DerivedA constructor, sizeof: {}", sizeof(*this));
        print_obj("DerivedA", this, value());
        std::println("End DerivedA constructor\n");
    }
};


namespace bad_case {

class DerivedB : public BaseB {
public:
    // 构造顺序: BaseB -> BaseA
    // 依赖关系: BaseB -> &a_ -> DerivedA -> BaseA
    // DerivedB(int a): BaseB(&obj_a_), obj_a_(a) {
    DerivedB(int a): obj_a_(a), BaseB(&obj_a_) {
        std::println("Start DerivedB constructor, sizeof: {}", sizeof(*this));
        print_obj("DerivedA", &obj_a_, obj_a_.value());
        print_obj("DerivedB",  this, value());
        std::println("End DerivedB constructor\n");
    }
    auto value_a() { return obj_a_.value(); }
private:
    DerivedA obj_a_;
};

/**
根据构造函数的实现: DerivedB::DerivedB(int a): obj_a_(a), BaseB(&obj_a_) { ... } , 我们期望由参数 a 构造
obj_a_, 然后由 obj_a_ 构造 BaseB， 这样最终 value_a() 和 value_b() 都是参数 a 的值，即我们预期
@expected: assert(obj_b.value_a() == a && obj_b.value() == obj_b.value_a())

程序输出为
@stdout:
Start bad_case
Start BaseB constructor, sizeof: 4
addressof BaseA: start 0x7ff7bfaec92c end 0x7ff7bfaec930, value: 32760
addressof BaseB: start 0x7ff7bfaec928 end 0x7ff7bfaec92c, value: 32760
addressof BaseB::b_: start 0x7ff7bfaec928 end 0x7ff7bfaec92c, value: 32760
End BaseB constructor

Start BaseA constructor, sizeof: 4
addressof BaseA: start 0x7ff7bfaec92c end 0x7ff7bfaec930, value: 2
addressof BaseA::a_: start 0x7ff7bfaec92c end 0x7ff7bfaec930, value: 2
End BaseA constructor

Start DerivedA constructor, sizeof: 4
addressof DerivedA: start 0x7ff7bfaec92c end 0x7ff7bfaec930, value: 2
End DerivedA constructor

Start DerivedB constructor, sizeof: 8
addressof DerivedA: start 0x7ff7bfaec92c end 0x7ff7bfaec930, value: 2
addressof DerivedB: start 0x7ff7bfaec928 end 0x7ff7bfaec930, value: 32760
End DerivedB constructor

addressof obj_b: start 0x7ff7bfaec928 end 0x7ff7bfaec930, value: 32760
obj_b.obj_a_: 2
obj_b.value() != obj_b.value_a()
End bad_case


根据上面输出，可以分析出:
1. 程序为 obj_b(DerivedB/BaseB) 分配内存 [0x7ff7bfaec928, 0x7ff7bfaec930)
    - class DerivedB: 8 bytes [0x7ff7bfaec928, 0x7ff7bfaec930)
        - base_class BaseB: 4 bytes [0x7ff7bfaec928, 0x7ff7bfaec92c)
            - member int b_: 4 bytes [0x7ff7bfaec928, 0x7ff7bfaec92c)
        - member DerivedA obj_a_: 4 bytes [0x7ff7bfaec92c, 0x7ff7bfaec930)
            - base_class BaseA: 4 bytes [0x7ff7bfaec92c, 0x7ff7bfaec930)
                - member int a_: 4 bytes [0x7ff7bfaec92c, 0x7ff7bfaec930)
2. 执行构造函数:
    a. BaseB constructor: 此时 BaseA 还没完成构造，BaseA 的 value 是随机值 32760，
        BaseB 的 value 由 BaseA 构造，所以 BaseB 的 value 也和 BaseA 一样是随机值 32760
    b. BaseA constructor: 将 value(a_) 赋值为 2
    c. DerivedA constructor: value 为 BaseA::a_, 其值为 2
    d. DerivedB constructor: value 为 BaseB::b_, 其值为 32760
3. 最终: obj_b 的 value 为 随机值 32760， obj_b.obj_a_ 的 value 为 2, obj_b.value() != obj_b.value_a(),
并不符合我们的预期
@expected: assert(obj_b.value_a() == a && obj_b.value() == obj_b.value_a())
*/

}  // namespace bad_case


namespace solution {

class ContainA {
public:
    auto value_a() { return obj_a_.value(); }
    ContainA(int a): obj_a_(a) {
        std::println("Start ContainA constructor, sizeof: {}", sizeof(*this));
        print_obj("DerivedA", &obj_a_, obj_a_.value());
        print_obj("ContainA",  this, value_a());
        std::println("End DerivedB constructor\n");
    }
    DerivedA obj_a_;
};
class DerivedB : public ContainA, public BaseB {
public:
    DerivedB(int a): ContainA(a), BaseB(&obj_a_) {
        std::println("Start DerivedB constructor, sizeof: {}", sizeof(*this));
        print_obj("ContainA", &obj_a_, obj_a_.value());
        print_obj("DerivedB",  this, value_a());
        std::println("End DerivedB constructor\n");
    }
};

/**
@stdout:
Start solution
Start BaseA constructor, sizeof: 4
addressof BaseA: start 0x7ff7b26d78bc end 0x7ff7b26d78c0, value: 2
addressof BaseA::a_: start 0x7ff7b26d78bc end 0x7ff7b26d78c0, value: 2
End BaseA constructor

Start DerivedA constructor, sizeof: 4
addressof DerivedA: start 0x7ff7b26d78bc end 0x7ff7b26d78c0, value: 2
End DerivedA constructor

Start BaseB constructor, sizeof: 4
addressof BaseA: start 0x7ff7b26d78bc end 0x7ff7b26d78c0, value: 2
addressof BaseB: start 0x7ff7b26d78c0 end 0x7ff7b26d78c4, value: 2
addressof BaseB::b_: start 0x7ff7b26d78c0 end 0x7ff7b26d78c4, value: 2
End BaseB constructor

addressof obj_b: start 0x7ff7b26d78bc end 0x7ff7b26d78c4, value: 2
obj_b.obj_a_: 2
obj_b.value() == obj_b.value_a()
End solution
 */

}  // namespace solution


template <class T>
void run(const std::string& name) {
    std::println("Start {}", name);
    int a{2};
    T obj_b(a);

    print_obj("obj_b", &obj_b, obj_b.value());

    std::println("obj_b.obj_a_: {}", obj_b.value_a());
    if (obj_b.value() == obj_b.value_a()) {
        std::println("obj_b.value() == obj_b.value_a()");
    } else {
        assert("solution" != name);
        std::println("obj_b.value() != obj_b.value_a()");
    }
    std::println("End {}\n", name);
}

#define RUN(name) run<name::DerivedB>(#name)
int main() {
    RUN(bad_case);
    RUN(solution);
    return 0;
}
