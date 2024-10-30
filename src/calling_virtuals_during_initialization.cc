/**
Derived 在继承 Base 后, 我们期望 Base 在构造时能调用 Derived 中的虚函数

但实际是: virtual_function in subclasses are not available in the constructor of 'Base'

@solution:
方法一: two-phase-initialization, 将 Base 构造中调用继承类虚函数的逻辑放到单独的 init 函数，并通过工厂方法构造 Derived obj 和调用 init 函数
    factory() {
        Derived obj;
        obj.init();
        return obj;
    }
方法二: 好像不太行
*/

#include <print>

#include "cpp_utils/util.h"

#define DERIVED_VIRTUAL_FUNCTION_DEFINE \
    virtual void foo(int n) const override { \
        std::println("{}, n * n = {}", CURRENT_FUNCTION_NAME, n * n); \
    } \
    virtual double bar() const override { \
        double bar{1000000.0}; \
        std::println("{}, bar: {}", CURRENT_FUNCTION_NAME, bar); \
        return bar; \
    } \
    virtual void base_is_pure_virtual() const override { \
        std::println("{}", CURRENT_FUNCTION_NAME); \
    }

namespace bad_case {

class Base {
public:
    Base() {
        auto function_name = CURRENT_FUNCTION_NAME;
        std::println("Start {}", function_name);
        foo(32);
        bar();
        base_is_pure_virtual();
        std::println("End {}\n", function_name);
    }
    // 如果是纯虚函数，那么构造函数 Base() 在调用 foo(32) 时将会编译出错
    // warning:
    //  call to pure virtual member function 'foo' has undefined behavior;
    //  overrides of 'foo' in subclasses are not available in the constructor of 'Base'
    virtual void foo(int n) const {
        std::println("{}, n + n = {}", CURRENT_FUNCTION_NAME, n + n);
    }
    virtual double bar() const {
        double bar{1.0};
        std::println("{}, bar: {}", CURRENT_FUNCTION_NAME, bar);
        return bar;
    }
    virtual void base_is_pure_virtual() const {
        std::println("{}", CURRENT_FUNCTION_NAME);
    }
};

class Derived : public Base {
public:
    DERIVED_VIRTUAL_FUNCTION_DEFINE
};

/**
@stdout:
Start bad_case::Base::Base()
virtual void bad_case::Base::foo(int) const, n + n = 64
virtual double bad_case::Base::bar() const, bar: 1
End bad_case::Base::Base()

*/
void run() {
    bad_case::Derived obj;
}

}  // namespace bad_case



namespace solution1 {

class Base {
public:
    void init() {
        foo(32);
        bar();
        base_is_pure_virtual();
    }
    virtual void foo(int n) const {
        std::println("{}, n + n = {}", CURRENT_FUNCTION_NAME, n + n);
    }
    virtual double bar() const {
        double bar{1.0};
        std::println("{}, bar: {}", CURRENT_FUNCTION_NAME, bar);
        return bar;
    }
    virtual void base_is_pure_virtual() const = 0;

    template <class D, class... Args>
    static std::shared_ptr<D> Create(Args... args) {
        // https://stackoverflow.com/questions/73081412/cant-access-private-constructor-from-a-friend-class
        // 构造函数是私有
        // auto ptr = std::make_shared<D>(args...);
        std::shared_ptr<D> ptr(new D(args...));
        static_cast<Base*>(ptr.get())->init();
        return nullptr;
    }
};


class Derived : public Base {
    friend void run();
    // https://stackoverflow.com/questions/73081412/cant-access-private-constructor-from-a-friend-class
    // need for factory
    friend Base;
    // friend std::shared_ptr<Derived> Base::Create<Derived>(int, char);
private:
    Derived(int i, char j) {
        std::println("{}, i: {}, j: {}", CURRENT_FUNCTION_NAME, i, j);
    }
public:
    DERIVED_VIRTUAL_FUNCTION_DEFINE

};

void run() {
    {
        // 其实这样也可以，只是把构造的逻辑拿出来单独执行了
        std::println("solution1: construct by init");
        Derived obj(1, 'x');
        obj.init();
    }
    std::println();
    {
        std::println("solution1: construct by factory");
        // 为了营造出在构造时调用的虚函数的现象，需要将构造和 init 封装在一起
        // 并将构造设置成 private，避免使用者误用(比如忘记init)

        // 通过 Base 中的 factory 函数
        auto obj = Base::Create<Derived>(2, 'y');
    }
    std::println();
}

}  // namespace solution1



// 下面代码是错的
/**
namespace solution2 {

class Base {
public:
    virtual void foo(int n) const {
        std::println("{}, n + n = {}", CURRENT_FUNCTION_NAME, n + n);
    }
    virtual double bar() const {
        double bar{1.0};
        std::println("{}, bar: {}", CURRENT_FUNCTION_NAME, bar);
        return bar;
    }
    virtual void base_is_pure_virtual() const = 0;
};

template <class D>
class Init : public Base {
protected:
    Init(D* ptr); //{
};

class Derived : public Init<Derived> {
public:
    Derived() : Init<Derived>(this) {}
public:
    DERIVED_VIRTUAL_FUNCTION_DEFINE

};
template <class D>
Init<D>::Init(D* ptr) {
        // 在构造函数里调用虚函数，都是调用的基类的虚函数，不符合我们的需求
        ptr->foo(42);
        ptr->bar();
        // 由于base_is_pure_virtual是基类的纯虚函数，这里在执行时会core
        // Pure virtual function called!
        ((Derived*)ptr)->base_is_pure_virtual();
    }

void run() {
    Derived obj;
}

}  // namespace solution2
*/

int main() {
    bad_case::run();
    solution1::run();
    // solution2::run();
    return 0;
}

