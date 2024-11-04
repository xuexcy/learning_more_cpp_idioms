/**
友元类可以访问原 class 的所有私有成员(方法), 但实际我们希望控制友元类只能访问部分(subset)私有成员
假设 class Client 允许 class Bar 访问其部分私有成员 A/B
如下 class ClientV1 将 class Bar 设置成友元类，这样 class Bar 就可以访问 class ClientV1 的 A/B
但是 class Bar 还可以访问 class ClientV1 的 C，超出了我们期望 class Client 允许 class Bar 访问的范围

@solution: class Client 将代理类 class Attorney 设置为 friend，通过 Attorney 来控制其他类对
class Client(friend Attorney);
class Bar(friend Attorney);
这样 class Bar 能访问哪些 class Client 的 private 成员可以用 class Attorney 来控制

c++ 中的友元不能继承
如果 Attorney 需要通过父类的友元关系来访问其子类的私有成员，只能是 virtual Base::function,并且该
virtual function 在子类中必须是 accessible 的,如 public 继承(class Derived : public Base)
*/

#include <print>

#include "cpp_utils/util.h"

#include "utils/main_decorator.h"

// #define GET_NAME_API(class_name) \
//     inline static std::string_view get_name() { return #class_name; }

namespace attorney {
class ClientV1 {
private:
    // GET_NAME_API(ClientV1)
    void A(int a) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("a: {}", a);
    }
    void B(float b) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("b: {}", b);
    }
    void C(double c) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("c: {}", c);
    }
    friend class Bar;
};

class ClientV2 {
private:
    void A(int a) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("a: {}", a);
    }
    void B(float b) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("b: {}", b);
    }
    void C(double c) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("c: {}", c);
    }
    friend class AttorneyAB;
    friend class AttorneyC;
};

// 代理类
class AttorneyAB{
private:
    static void callA(ClientV2& c, int a) {
        c.A(a);
    }
    static void callB(ClientV2& c, float b) {
        c.B(b);
    }
    friend class Bar;
};

class AttorneyC {
private:
    static void callC(ClientV2& client, double c) {
        client.C(c);
    }
    friend class AccessC;
};

class Bar {
public:
    void access_client_v1() {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        ClientV1 client;
        client.A(1);
        client.B(2.0f);
        client.C(3.0);
    }

    void access_client_v2_by_attorney() {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        ClientV2 client;
        AttorneyAB::callA(client, 1);
        AttorneyAB::callB(client, 2.0f);
    }
};

class AccessC {
public:
    void access_client_v2_by_attorney() {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        ClientV2 client;
        AttorneyC::callC(client, 2.0);

    }
};

}  // namespace attorney











int main();

namespace attorney_inherit {

class Base {
private:
    virtual void Func(int x) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("x: {}", x);
    };
    friend class Attorney;
public:
    virtual ~Base() {}
};

class Derived : public Base {
private:
    virtual void Func(int x) {
        std::println("call {} ", CURRENT_FUNCTION_NAME);
        std::println("x: {}", x);
    }
};


class Attorney {
private:
    static void callFunc(Base& b, int x) {
        b.Func(x);
    }
    friend int ::main();
};

}  // namespace attorney_inherit

int main() {
    utils::MainDecorator::Access();
    attorney::Bar bar;
    bar.access_client_v1();
    bar.access_client_v2_by_attorney();

    attorney::AccessC c;
    c.access_client_v2_by_attorney();

    attorney_inherit::Base b;
    attorney_inherit::Derived d;
    attorney_inherit::Attorney::callFunc(b, 5);
    attorney_inherit::Attorney::callFunc(d, 10);
}
