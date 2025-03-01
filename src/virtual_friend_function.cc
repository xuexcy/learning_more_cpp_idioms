/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/03/01 23:26:22
# Desc   :
########################################################################
friend 函数不能是虚函数，为了实现实现友元函数多态，可以让友元函数去调用虚函数
*/

#include <iostream>
#include <ostream>
#include "utils/main_decorator.h"

class Base {
public:
  friend std::ostream& operator<<(std::ostream& os, const Base& b);
protected:
  virtual void print(std::ostream& os) const {
    os << "Base";
  }
};  // class Base

inline std::ostream& operator<<(std::ostream& os, const Base& b) {
  b.print(os);
  return os;
}

class Derived : public Base {
protected:
  virtual void print(std::ostream& os) const override {
    os << "Derived";
  }
};  // class Derived

void run() {
  Base b;
  Derived d;
  std::cout << b << std::endl;
  std::cout << d << std::endl;
}

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}

