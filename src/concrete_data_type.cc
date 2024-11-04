/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/04 18:47:49
# Desc    :
########################################################################

有两种类实例(class instance/class object):
    1. 在桟上分配内存的变量，变量超出作用域后被析构并回收内存
    2. 在堆上 new 出来的对象，变量超出作用域前需要自己 delete 来回收内存，否则会造成内存泄漏
我们希望通过是否允许动态分类内存在控制 object 的作用域和生命周期
case 1: 希望某个类只能在堆上分配内存
    思路: 将构造函数或析构函数设置成 protected，这样就不能直接在桟上构造或析构该类的实例
    solution 1: 多态(基类虚析构函数) + 子类 protected destructor
    solution 2: protected constructor + class static new_instance_function
    总结, solution 1 用的办法是不能直接析构(只能使用多态)， solution 2 用的办法是不能直接构造(只能通过 static 函数来构造)
case 2: 希望某个类只能在桟上分配内存
    思路: 不让使用 new
    solution: 将 new(size_t) 和 new(size_t, void* mem) 设置成 private，以此禁止使用 new 和 placement new

总结: 两种 case 的 solution 都是通过 protected 或 private 来禁止使用某个功能，比如构造、析构、new、placement new

*/

#include <cstddef>

#include "utils/main_decorator.h"

namespace must_in_heap_solution_1 {
/**
解释如下:
1. 定义基类 EventHandler ，析构函数定义为虚函数
2. 定义实际想要控制只能在对上分配内存的子类 MouseEventHandler ，将析构函数设置为 protected
3. 由于子类的析构函数为 protected，所以不能直接定义一个变量 MouseEventHandler m;
4. MouseEventHandler 的实例的析构只能通过多态的方式实现(delete base_ptr), 即 delete EventHandler*
 */
class EventHandler {
public:
    virtual ~EventHandler() {}
};

class MouseEventHandler : public EventHandler {
protected:
    ~MouseEventHandler() {}
public:
    MouseEventHandler() {}
};

void run() {
    // no: Variable of type '' has protected destructor
    // MouseEventHandler m;

    // yes
    EventHandler *e = new MouseEventHandler();
    delete e;
}
}  // namespace must_in_head_solution_1
namespace must_in_heap_solution_2 {
/**
1. 将构造函数设置成 protected，这样就不能在桟上直接构造类实例
2. 通过一个 static member_function 从桟上 new 一个实例
 */
class MouseEventHandler {
protected:
    MouseEventHandler() {}
public:
    ~MouseEventHandler() {}
    static MouseEventHandler* new_instance() {
        return new MouseEventHandler();
    }
};

class MouseEventHandler2 {
protected:
    MouseEventHandler2() {}
    ~MouseEventHandler2() {}
public:
    static MouseEventHandler2* new_instance() {
        return new MouseEventHandler2();
    }
    void destroy() {
        delete this;
    }
};

void run() {
    // no: Calling a protected constructor of class 'must_in_heap_solution_2::MouseEventHandler'
    // MouseEventHandler m;

    // yes
    MouseEventHandler* m1 = MouseEventHandler::new_instance();
    delete m1;

    // yes
    MouseEventHandler2* m2 = MouseEventHandler2::new_instance();
    m2->destroy();
}
}  // namespace must_in_heap_solution_2


namespace must_in_stack_solution {
class MouseEventHandler {
private:
    // 不允许使用 new
    static void* operator new(size_t size);
    // 不允许使用 placement new
    static void* operator new(size_t, void* mem);
};

void run() {
    {
        // no: 'operator new' is a private member of 'must_in_stack_solution::MouseEventHandler'
        // 不允许使用 new
        // MouseEventHandler* m = new MouseEventHandler();
    }
    {
        // 不允许使用 placement new
        void* buf = ::operator new (sizeof(MouseEventHandler));
        // MouseEventHandler* m = new(buf) MouseEventHandler();
    }
    {
        // yes
        MouseEventHandler m;
    }

}
}  // namespace must_in_stack_solution

int main() {
    utils::MainDecorator::Access();
    must_in_heap_solution_1::run();
    must_in_heap_solution_2::run();

    must_in_stack_solution::run();
    return 0;
}



