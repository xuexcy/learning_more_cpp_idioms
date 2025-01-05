/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/05 14:41:54
# Desc   :
########################################################################
通过将拷贝构造/赋值设置成 private，来禁止一些类的实例被拷贝或赋值（比如一个网络连接）

https://www.boost.org/doc/libs/1_78_0/boost/core/noncopyable.hpp
另外，在 boost::noncopyable 中，有一个 struct base_token，这个类可以用来在判断类是否继承自 class noncopyable 时，
不需要 noncopyable 的完整定义（只需要 base_token 的定义就行），这样可以减少编译依赖
*/

namespace solution {
class NoCopyable {
private:
  NoCopyable(const NoCopyable&) = delete;
  NoCopyable& operator=(const NoCopyable&) = delete;
protected:
  NoCopyable() = default;  // 不加这个默认构造函数，class A 将不能定义默认构造函数

};  // NoCopyable
class A : private NoCopyable {
public:
  A() = default;

};  // class A
}  // namespace solution

void run() {
  solution::A a;

  // 无法编译
  // solution::A aa(a);

  solution::A b;
  // 无法编译
  // b = a;
}
int main() {

  return 0;
}



