/**
通过将接口函数放到基类，并声明为纯虚函数，来实现接口类 class Interface, 如下 class shape


 */
#include <memory>
#include <print>
#include <vector>

#include "utils/main_decorator.h"

class shape {
public:
    virtual ~shape() {}
    virtual void move_x(int x) = 0;
    virtual void move_y(int y) = 0;
    virtual void draw() = 0;
};

class point : public shape {
public:
    point(int x, int y): x_(x), y_(y) {}
    virtual ~point() = default;
    virtual void move_x(int x) override {
        x_ += x;
    }
    virtual void move_y(int y) override {
        y_ += y;
    }
    virtual void draw() override {
        std::println("point: ({}, {})", x_, y_);
    }
private:
    int x_{0};
    int y_{0};
};

class line : public shape {
public:
    line(point start, point end): start_(start), end_(end) {}
    virtual ~line() = default;
    virtual void move_x(int x) override {
        start_.move_x(x);
        end_.move_x(x);
    }
    virtual void move_y(int y) override {
        start_.move_y(y);
        end_.move_y(y);
    }
    virtual void draw() override {
        std::println("line:");
        std::print("    start: ");
        start_.draw();
        std::print("    end: ");
        end_.draw();
    }
private:
    point start_;
    point end_;
};

int main() {
    std::vector<std::shared_ptr<shape>> shapes;
    shapes.push_back(std::make_shared<point>(0, 0));
    shapes.push_back(std::make_shared<line>(point(0, 0), point(0, 4)));
    for (auto shape : shapes) {
        shape->draw();
    }
    shapes[0]->move_y(3);
    shapes[1]->move_y(4);
    for (auto shape : shapes) {
        shape->draw();
    }
    return 0;
}
