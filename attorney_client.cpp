// 友元类可以访问原 class 的所有私有成员(方法), 但实际我们希望控制友元类只能访问部分(subset)私有成员
// 假设 class Client 允许 class Bar 访问其部分私有成员 A/B
// 如下 class ClientV1 将 class Bar 设置成友元类，这样 class Bar 就可以访问 class ClientV1 的 A/B
// 但是 class Bar 还可以访问 class ClientV1 的 C，超出了我们期望 class Client 允许 class Bar 访问的范围

// solution: class Client 将代理类 class Attorney 设置为 friend，通过 Attorney 来控制其他类对
// class Client(friend Attorney);
// class Bar(friend Attorney);
// 这样 class Bar 能访问哪些 class Client 的 private 成员可以用 class Attorney 来控制

#include <print>
#include <source_location>

#define CURRENT(method_name) \
    std::source_location::current().method_name()
// #define GET_NAME_API(class_name) \
//     inline static std::string_view get_name() { return #class_name; }
class ClientV1 {
private:
    // GET_NAME_API(ClientV1)
    void A(int a) {
        std::println("call {} ", CURRENT(function_name));
        std::println("a: {}", a);
    }
    void B(float b) {
        std::println("call {} ", CURRENT(function_name));
        std::println("b: {}", b);
    }
    void C(double c) {
        std::println("call {} ", CURRENT(function_name));
        std::println("c: {}", c);
    }
    friend class Bar;
};

class ClientV2 {
private:
    void A(int a) {
        std::println("call {} ", CURRENT(function_name));
        std::println("a: {}", a);
    }
    void B(float b) {
        std::println("call {} ", CURRENT(function_name));
        std::println("b: {}", b);
    }
    void C(double c) {
        std::println("call {} ", CURRENT(function_name));
        std::println("c: {}", c);
    }
    friend class Attorney;
};

// 代理
class Attorney {
private:
    static void callA(ClientV2& c, int a) {
        c.A(a);
    }
    static void callB(ClientV2& c, float b) {
        c.B(b);
    }
    friend class Bar;
};

class Bar {
public:
    void access_client_v1() {
        ClientV1 client;
        client.A(1);
        client.B(2.0f);
        client.C(3.0);
    }

    void access_client_v2_by_attorney() {
        ClientV2 client;
        Attorney::callA(client, 1);
        Attorney::callB(client, 2.0f);
    }
};

int main() {
    Bar bar;
    bar.access_client_v1();
    bar.access_client_v2_by_attorney();
}
