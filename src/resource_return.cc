/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 14:04:10
# Desc   :
########################################################################
当通过函数返回指针时，由谁来管理资源，如果交给用户，用户可能会忘记释放资源

@solution:
将函数返回的裸指针换成智能指针
*/


#include <memory>
struct X {
  void foo() {}
};  // struct X

namespace bad_case {
X* XFactory() {
  return new X();
}
}  // namespace bad_case

namespace solution {
std::unique_ptr<X> XFactory() {
  return std::make_unique<X>();
}
}  // namespace solution

void run() {
  bad_case::XFactory()->foo();  // 内存泄露，没有 delete X_ptr
  solution::XFactory()->foo();
}


int main() {
  run();
  return 0;
}
