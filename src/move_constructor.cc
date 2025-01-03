/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/02 23:24:14
# Desc   :
########################################################################
在 c++ 11 以前， 通过将拷贝构造函数的参数设置为 non-const，就可以实现资源转移，也就是 move constructor
但是, 当函数返回的对象，又作为其他函数的参数时，这个(临时)对象不能绑定到 non-const 参数（修改临时对象没有意义）。 这样，
就不能调用 non-const 的拷贝构造函数接受这个临时对象。那么，就不能将临时对象的资源通过 non-const 拷贝构造函数转移给其他对象。

c++ features:
1. 拷贝构造函数的参数不需要是 const，只需要第一个参数是 X&, const X&, volatile X& or const volatile
X& 的类型(这几个类型都是 reference)并且其他参数都有默认值
2. 编译器会自动识别隐式转换
3. 临时对象可以调用 non-const 函数

@solutions:
利用上面的 3 个 features，通过:
  "函数返回临时对象 MovableData<T> tmp"
  -> "隐式转换 proxy<T> p"
  -> "non-const 构造函数 MoveableData(proxy<T>)"
来实现 move constructor
*/

#include <iostream>
#include <utility>

template <typename T>
struct proxy {
  proxy(T* _t): t(_t) {
    std::cout << "proxy 带参构造\n";
  }
  T* t;
};  // struct proxy


namespace move_constructor {  // bad
template <typename T>
struct MoveableData {
  MoveableData(T* t) : t_(t) { std::cout << "MoveableData 带参构造: MoveableData(T*)\n"; }
  ~MoveableData() {
    std::cout << "Moveable 析构\n";
    delete t_;
  }
  // 当需要资源 moveable 的时候，拷贝构造的参数必须是 non-const，否则无法移交资源 T* t_
  // 但是临时返回的对象不能当成 non-const 的参数传递，所以需要通过 proxy 来实现
  MoveableData(MoveableData& other) : t_(other.t_) {
    std::cout << "MoveableData 拷贝构造(non-const 参数): MoveableData(MoveableData&)\n";
    other.t_ = NULL;
  }
  MoveableData(proxy<T> p) : t_(p.t) {
    std::cout << "Moveable 带参构造(proxy<T>): Moveable(proxy<T>)\n";
  }
  MoveableData& operator=(MoveableData& other) {
    std::cout << "MoveableData 拷贝赋值(non-const 参数): MoveableData& operator=(Moveable&)\n";
    if (this == &other) {
      return *this;
    }
    MoveableData tmp(other);
    tmp.swap(*this);
    return *this;
  }
  MoveableData& operator=(proxy<T> p) {
    std::cout << "MoveableData 拷贝赋值(proxy 参数): MoveableData& operator=(proxy<T>)\n";
    MoveableData tmp(p);
    tmp.swap(*this);
    return *this;
  }
  operator proxy<T>() {
    std::cout << "MoveableData 隐式转换成 proxy<T>: operator proxy<T>()\n";
    proxy<T> p(t_);
    t_ = NULL;
    return p;
  }

private:
  void swap(MoveableData& other) { std::swap(t_, other.t_); }

private:
  T* t_;
};  // struct MoveableData

MoveableData<int> fun() {
  return MoveableData<int>(new int(42));
}

// 对临时对象的一次拷贝流程:
// 1. 带参构造(T*): MoveableData<int> data(new int(42));
// 2. 隐式转换 + 带参构造(proxy): operator proxy<T>() + MoveableData(proxy<T>)，由于 fun
//   返回的对象不能绑定到 non-const 参数，所以 MoveableData<int> data(fun()); 会调用
//   MoveableData(proxy<T> p)
// 可以看到，对于临时对象，我们并没有调用 non-const 拷贝构造函数，但是实现了资源转移

/**
gcc-5.1 --no-elide-constructors
@stdout:
MoveableData 带参构造: MoveableData(T*)
MoveableData 隐式转换成 proxy<T>: operator proxy<T>()
proxy 带参构造
Moveable 带参构造(proxy<T>): Moveable(proxy<T>)
Moveable 析构
MoveableData 隐式转换成 proxy<T>: operator proxy<T>()
proxy 带参构造
Moveable 带参构造(proxy<T>): Moveable(proxy<T>)
Moveable 析构
Moveable 析构
*/
void run() {
  // 在编译参数没有 --no-elide-constructor 时，会有 RVO (return value optimization)
  // 没有 RVO 时，会发生两次拷贝，一次时 fun() 里的 MoveableData<int>(new int(42)) 拷贝到函数返回的临时对象，一次是这个临时对象拷贝到 data
  MoveableData<int> data(fun());
}

}  // namespace move_constructor


namespace safe_move_constructor {  // good

void run_copy_assignment() {
  using namespace move_constructor;
  {
    MoveableData<int> data(new int(42));
    MoveableData<int> data2(NULL);
    data2 = data;  // 语法是 copy assignment，但实际上是发生了 move assignment
  }
  {
    MoveableData<int> data(new int(42));
    MoveableData<int> data2(data);  // 语法是 copy constructor，但实际上是发生了 move constructor
  }
}

// 对于 move_constructor::MoveableData 这个类，copy constructor/assignment 都是在 move
// 我们不应该让 copy 语法发生 move
// @solution: 将 copy 全部设置为 private，并提供额外的 move 函数
template <typename T>
struct MoveableData {
  MoveableData(T* t) : t_(t) { std::cout << "MoveableData 带参构造: MoveableData(T*)\n"; }
  ~MoveableData() {
    std::cout << "Moveable 析构\n";
    delete t_;
  }
  MoveableData(proxy<T> p) : t_(p.t) {
    std::cout << "Moveable 带参构造(proxy<T>): Moveable(proxy<T>)\n";
  }
  MoveableData& operator=(proxy<T> p) {
    std::cout << "MoveableData 拷贝赋值(proxy 参数): MoveableData& operator=(proxy<T>)\n";
    MoveableData tmp(p);
    tmp.swap(*this);
    return *this;
  }
  operator proxy<T>() {
    std::cout << "MoveableData 隐式转换成 proxy<T>: operator proxy<T>()\n";
    proxy<T> p(t_);
    t_ = NULL;
    return p;
  }

private:
  MoveableData(MoveableData& other);
  MoveableData& operator=(MoveableData& other);
  void swap(MoveableData& other) { std::swap(t_, other.t_); }
private:
  T* t_;
  // 这个模板 friend 函数最好定义在类里面，否则会有链接错误（见 making_new_friends.cc ）
  friend MoveableData<T> move(MoveableData<T>& data) {
    return MoveableData<T>(proxy<T>(data));  // 先隐式转换，再带参构造
  }
};  // struct MoveableData

MoveableData<int> get_a_data() {
  MoveableData<int> local(new int(42));
  return move(local);
}

void move_data_away(MoveableData<int> data) {

}

void run() {
  std::cout << std::endl;
  std::cout << "void safe_move_constructor::run_copy_assignment()\n";
  run_copy_assignment();
  std::cout << std::endl;
  {
    MoveableData<int> data(get_a_data());
    MoveableData<int> data2(NULL);
    // data2 = data;  // copy assignment is private
    data2 = move(data);  // 主动调用 move
    // move_data_away(data);  // copy constructor is private
    move_data_away(move(data));
  }
}
}  // namespace safe_move_constructor

int main() {
  move_constructor::run();
  safe_move_constructor::run();
  return 0;
}
