/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/06 21:22:40
# Desc   :
########################################################################
在模板类 List 中，有一个 Alloc 模板参数用来指定分配内存的策略，比如 Allocator<T>，但是在 List 中需要分配内存的
类型是 Node<T>，也就是我们需要 Allocator<Node<T>>。如何在指定 Alloc 时，将对应的分配类型参数从 T 变成 Node<T>

@solution:
1. 在策略类(如下的 Allocator 模板类)中定义一个 rebind 类，接受和外部策略相同的模板参数
2. 在使用这个策略的类(如下的 List 类)中 使用 rebind 重新为策略类绑定新的模板参数(如下将 Allocator<T>
改为 Allocator<Node<T>>)
*/

namespace bad_case {

template <class T>
class Allocator {};  // class Allocator

template <class T>
class Node {
};  // class Node
template <class T, class Alloc=Allocator<T>>
class List {
  // no: Alloc 不是模板
  // using other = Alloc<Node<T>>;
};  // class List

template <class T, template <class U> class Alloc=Allocator>
class List2 {
  // 可能有些比较老旧的编译器不支持模板参数是模板
  using other = Alloc<Node<T>>;
};  // class List

}  // namespace bad_case




namespace solution {
template <class T>
class Allocator {
public:
  template <class U>
  struct rebind {
    using other = Allocator<U>;
  };  // struct rebind

  // 另一种简洁写法
  template <class U>
  using rebind2 = Allocator<U>;
};  //  Allocator

template <class T>
class Node {
};  // class Node

template <class T, class Alloc=Allocator<T>>
class List {
  // 这里限制了 List 为 Node 使用的分配器，必须和 T 使用的分配器是同一个'类型'
  using other = Alloc::template rebind<Node<T>>::other;
  using other2 = Alloc::template rebind2<Node<T>>;
};  // class List

}  // namespace solution

namespace why_use_rebind {
template <typename T>
struct MyAllocator {
  template <class U>
  using rebind = MyAllocator<U>;
};

using IntAllocator = MyAllocator<int>;


// 假设我们需要使用 IntAllocator，并将模板参数从 int 改为 float 和 double，有两种方式
// 1. 直接像 IntAllocator 一样定义 FloatAllocator，将模板参数设置成 float
using FloatAllocator = MyAllocator<double>;
// 2. 通过 IntAllocator 的 rebind 来将模板参数修改为 double
using DoubleAllocator = IntAllocator::rebind<double>;

// 如果 IntAllocator 的定义发生变化，比如变成 HisAllocator<int>，或者改了一下名字，比如 my_allocator<int>，
// 我们需要同步修改 FloatAllocator 的定义成 HisAllocator<float> 或 my_allocator<float>
// 但是我们不需要修改 DoubleAllocator，因为它是通过 IntAllocator rebind 得到的，rebind 使用的模板类和定义
// IntAllocator 的模板类是同一个，这点我们已经通过 rebind 的定义 保证了

// 另外，如果 IntAllocator 的定义比较复杂，比如 namespace 层数多，名字长，通过这种方式也可以代码量
}  // namespace why_use_rebind

int main() {
  return 0;
}
