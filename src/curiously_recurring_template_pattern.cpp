/**
奇异递归模板模式

常见的多态通过虚函数在运行期实现多态，通过 虚指针->虚函数表->虚函数 的方式实现
crtp: 派生类作为基类的模板参数，在基类中通过 static_cast<T*>(this) 的方式来获取继承类指针，以此实现编译期多态
 */

#include <cassert>
#include <memory>
#include <print>

#include "utils/main_decorator.h"

namespace virtual_polymorphism {
class Base {
public:
    virtual std::string Run() {
        std::println("virtual_polymorphism::Base::Run()");
        return "virtual_polymorphism::Base";
    }
};

class Derived : public Base {
public:
    std::string Run() override {
        std::println("virtual_polymorphism::Derive::Run()");
        return "virtual_polymorphism::Derived";
    }
};

}  // namespace virtual_polymorphism

namespace crtp_polymorphism {

template <typename T>
class Base {
public:
    T* cast() { return static_cast<T*>(this); }
    std::string Run() { return cast()-> Run(); }
};

class Derived: public Base<Derived> {
public:
    std::string Run() {
        std::println("crtp_polymorphism::Derived::Run()");
        return "crtp_polymorphism::Derived";
    }
};

}  // namespace crtp_polymorphism

int main() {
    namespace vp = virtual_polymorphism;
    namespace cp = crtp_polymorphism;

    auto v_b = std::make_unique<vp::Base>();
    decltype(v_b) v_d = std::make_unique<vp::Derived>();
    assert("virtual_polymorphism::Base" == v_b->Run());
    assert("virtual_polymorphism::Derived" == v_d->Run());

    // 1. crtp 这种方式实现的多态只能等同于带有纯虚函数的多态， 因为无法构造 cp::Base 的对象
    // 如下无法编译
    // auto c_b = std::make_unique<cp::Base<cp::Base<>>();

    // 2. 注释掉 Derived 的 Run 函数，程序还可以编译，但是运行 c_d->Run() 时会 segmentation fault
    // 因为 Base* 通过 static_cast 转换成 Derived* 后找不到 Derived::Run()
    // 但是通过纯虚函数实现的多态，如果 Derived 类不实现虚函数，就会无法编译，代码相对安全
    auto c_d = std::make_unique<cp::Base<cp::Derived>>();
    assert("crtp_polymorphism::Derived" == c_d->Run());
    return 0;
}
