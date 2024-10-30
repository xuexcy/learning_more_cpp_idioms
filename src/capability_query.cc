/**
在运行时判断某个 obj/class 是否满足某个接口

@solution: 使用 dynamic_cast 将继承类对象指针转换为基类指针，通过判断转换是否成功来判断 obj 是否满足某个接口
 */

#include <memory>

#include "utils/main_decorator.h"

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Rollable {
public:
    virtual ~Rollable() = default;
    virtual void roll() const = 0;
};

class Circle : public Shape, public Rollable {
public:
    void draw() const override {}
    void roll() const override {}
};

class Square : public Shape {
public:
    void draw() const override {}
};

std::shared_ptr<Shape> get_shape(int n) {
    std::shared_ptr<Shape> s_ptr{nullptr};
    if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0) {
        s_ptr.reset(new Circle());
    } else {
        s_ptr.reset(new Square());
    }
    return s_ptr;
}

template <class T>
void roll_it(T* ptr) {
    if (Rollable* r_ptr = dynamic_cast<Rollable*>(ptr)) {
        std::println("Rollable");
        r_ptr->roll();
    } else {
        std::println("not Rollable");
    }
}

int main() {
    roll_it(get_shape(2).get());
    roll_it(get_shape(7).get());
    roll_it((Shape*)nullptr);
    return 0;
}
