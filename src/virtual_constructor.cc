/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/02/28 20:52:15
# Desc   :
########################################################################
构造函数和拷贝构造函数无法为虚函数，因为构造需要在编译期就知道其具体类型

@solution:
  1. 在基类中声明纯虚函数 create() 和 clone()，分别用于构造和拷贝
  2. 在继承类中实现这两个函数，返回一个指向新对象的指针

employee_2 相对于 employee 来说，使用 resource_return.cc 中的方式来管理资源，即 create() 和
clone() 返回一个智能指针
*/

#include "cpp_utils/util.h"
#include "utils/main_decorator.h"

namespace employee {
class Employee {
public:
  virtual ~Employee() = default;
  virtual Employee* create() const = 0;
  virtual Employee* clone() const = 0;
};  // class Employee

class Manager : public Employee {
public:
  Manager() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Manager(const Manager&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Manager* create() const override {
    return new Manager();
  }
  Manager* clone() const override {
    return new Manager(*this);
  }
};  // class Manager

class Staff : public Employee{
public:
  Staff() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Staff(const Staff&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Staff* create() const override {
    return new Staff();
  }
  Staff* clone() const override {
    return new Staff(*this);
  }
};  // class Staff

void construct_and_copy(const Employee& e) {
  // 在多态的情况下，我们没有办法直接使用虚构造函数和虚拷贝构造函数
  // 这里，我们通过
  Employee* from_construct = e.create();
  Employee* from_copy = e.clone();
  delete from_construct;
  delete from_copy;
}

void run() {
  Manager m;
  Staff s;
  construct_and_copy(m);
  construct_and_copy(s);
}
}  // namespace
namespace employee_2 {
class Employee {
public:
  using Ptr = std::shared_ptr<Employee>;
  virtual ~Employee() = default;
  virtual Ptr create() const = 0;
  virtual Ptr clone() const = 0;
};  // class Employee

class Manager : public Employee {
public:
  Manager() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Manager(const Manager&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Ptr create() const override {
    return Ptr(new Manager());
  }
  Ptr clone() const override {
    return Ptr(new Manager(*this));
  }
};  // class Manager

class Staff : public Employee{
public:
  Staff() {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Staff(const Staff&) {
    PRINT_CURRENT_FUNCTION_NAME;
  }
  Ptr create() const override {
    return Ptr(new Staff());
  }
  Ptr clone() const override {
    return Ptr(new Staff(*this));
  }
};  // class Staff

void construct_and_copy(const Employee& e) {
  Employee::Ptr from_construct = e.create();
  Employee::Ptr from_copy = e.clone();
  // 通过 resource_return.cc 中的方式来管理资源
  // delete from_construct;
  // delete from_copy;
}

void run() {
  Manager m;
  Staff s;
  construct_and_copy(m);
  construct_and_copy(s);
}

}  // namespace employee_2
int main() {
  utils::MainDecorator::Access();
  employee::run();
  employee_2::run();
  return 0;
}
