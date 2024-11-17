/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/17 13:55:45
# Desc    :
########################################################################
基类虚函数返回 A* ，子类继承重写后，想要返回 B*(B 继承 A) ，可以直接返回 B*(c++ 允许重写的函数返回的类型)，而不用
先返回 A* 在通过 dynamic_cast<B*> 进行使用
*/

#include "utils/main_decorator.h"
class A {
public:
    virtual void print() { std::println("A"); }
};
class B : public A {
public:
    virtual void print() { std::println("B"); }
};

class C {
public:
    void print() { std::println("C"); }
};
class D : public C {
public:
    void print() { std::println("D"); }
};

class Base {
public:
    virtual Base* clone() const {
        return new Base(*this);
    }
    virtual A* data() const {
        return new A();
    }
    virtual C* value() const {
        return new C();
    }
};


namespace bad_case {
class Derived : public Base {
public:
    virtual Base* clone() const override {
        return new Derived(*this);
    }
    virtual A* data() const override {
        return new B();
    }
    virtual C* value() const override {
        return new D();
    }
};
void run() {
    Derived* d1 = new Derived();
    {
        Base* b = d1->clone();
        Derived* d2 = dynamic_cast<Derived*>(b);
        if (d2) {
            std::println("get Derived* from Base* by dynamic_case");
        }
        delete d2;
    }
    {
        A* data_a = d1->data();
        data_a->print();  // output B because of polymorphism
        B* data_b = dynamic_cast<B*>(data_a);
        data_b->print();
        delete data_b;
    }
    {
        C* value_c = d1->value();
        value_c->print();  // output C because of no polymorphism
        D* value_d = static_cast<D*>(value_c);
        value_d->print();
        delete value_d;
    }
    delete d1;
}
}  // namespace bad_case

namespace solution {
class Derived : public Base {
public:
    virtual Derived* clone() const override {
        return new Derived(*this);
    }
    virtual B* data() const override {
        return new B();
    }
    virtual D* value() const override {
        return new D();
    }
};
void run() {
    Derived* d1 = new Derived();
    {
        // Base* b = d1->clone();
        // Derived* d2 = dynamic_cast<Derived*>(b);
        Derived* d2 = d1->clone();
        if (d2) {
            std::println("get Derived* from Base* by override function return sub-type of base-type");
        }
        delete d2;
    }
    {
        // A* data_a = d1->data();
        // data_a->print();
        // B* data_b = dynamic_cast<B*>(data_a);
        B* data_b = d1->data();
        static_cast<A*>(data_b)->print();  // output B
        data_b->print();
        delete data_b;
    }
    {
        D* value_d = d1->value();
        static_cast<C*>(value_d)->print();  // output C
        value_d->print();
        delete value_d;
    }
    delete d1;
}
}  // namespace bad_case

int main() {
    utils::MainDecorator::Access();
    bad_case::run();
    solution::run();
    return 0;
}

