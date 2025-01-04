/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/03 23:02:42
# Desc   :
########################################################################
使用 SFINAE 来检测 class 是否有某个成员或函数
*/
#include <print>
namespace solution {
template <class T>
class Detect_X {
  struct Fallback {
    int X;
  };
  struct Derived : T, Fallback {};
  using SizeOne = char[1];
  using SizeTwo = char[2];

  // 如果 T 有 X 成员，则 U::X 会有两个(一个来自 T，一个来自 Fallback)，那么就是产生歧义，无法匹配这个 func
  template <class U>
  static SizeOne& func(decltype(U::X)*);
  template <class U>
  static SizeTwo& func(...);

public:
  static constexpr bool value = (sizeof(func<Derived>(0)) == 2);
};  // class DetectX

class NoX {};
class HasX {
  double X{0.0};
};

void run() {
  std::println("class NoX has X: {}", Detect_X<NoX>::value);
  std::println("class HasX has X: {}", Detect_X<HasX>::value);
}
}  // namespace solution

/**
detecting_member 和 detecting_type 主要通过继承 T 和 Fallback，并在 Fallback 中声明要检测的 member 或 type,
通过判断 Derived 中是否有多个 member 或 type 来判断 T 是否有某个 member 或 type。
如果有多个 member 或 type，那么就会产生歧义，无法匹配到 func，从而判断 T 是否有某个 member 或 type。
 */
namespace detecting_member {
#define GENERATE_HAS_MEMBER(member)                                                \
  template <class T>                                                               \
  class HasMember_##member {                                                       \
    struct Fallback {                                                              \
      int member;                                                                  \
    };                                                                             \
    struct Derived : T, Fallback {};                                               \
    using NO = char[1];                                                            \
    using YES = char[2];                                                           \
    template <class U>                                                             \
    static NO& func(decltype(U::member)*);                                         \
    template <class U>                                                             \
    static YES& func(...);                                                         \
                                                                                   \
  public:                                                                          \
    static constexpr bool value = (sizeof(func<Derived>(nullptr)) == sizeof(YES)); \
  };                                                                               \
  template <class T>                                                               \
  constexpr bool has_member_##member = HasMember_##member<T>::value;

GENERATE_HAS_MEMBER(attr);
GENERATE_HAS_MEMBER(func);

struct A {
  int attr;
  void func() {}
};  // struct A

struct B {
  char att[3];
  double func(const char*);
};  // struct B

struct C : A, B {};

void run() {
  std::println("C has member attr:  {}", has_member_attr<C>);
  std::println("C has member func:  {}", has_member_func<C>);
}
}  // namespace detecting_member

namespace detecting_type {
#define GENERATE_HAS_TYPE(Type)                                                    \
  template <class T>                                                               \
  class HasType_##Type {                                                           \
    struct Fallback {                                                              \
      struct Type {};                                                              \
    };                                                                             \
    struct Derived : T, Fallback {};                                               \
    using NO = char[1];                                                            \
    using YES = char[2];                                                           \
    template <class U>                                                             \
    static NO& func(U::Type*);                                                     \
    template <class U>                                                             \
    static YES& func(...);                                                         \
                                                                                   \
  public:                                                                          \
    static constexpr bool value = (sizeof(func<Derived>(nullptr)) == sizeof(YES)); \
  };                                                                               \
  template <class T>                                                               \
  constexpr bool has_type_##Type = HasType_##Type<T>::value;

GENERATE_HAS_TYPE(Foo);

struct A {
  struct Foo;
};

struct B {
  using Foo = int;
};

struct C : A, B {};
void run() {
  std::println("C has type Foo:  {}", has_type_Foo<C>);
}

}  // namespace detecting_type

/**
和 detecting_member 、detecting_type 不同，detecting_function 不能通过继承 T 和 Fallback 的方式来解决。
检测 T 的是否有 ReturnType func_name(Args...) 时，这个函数指针类型是 ReturnType (T::*)(Args...)， 但是按
detecting_function::bad_case 那样去做，Fallback 中的函数的指针类型是 ReturnType (Fallback::*)(Args...)，
使用 Check<Derived> 时匹配的是 ReturnType (Derived::*)(Args...)，这样导致 T::*、Fallback::*、Derived::* 三者
的函数指针类型不一致，永远无法匹配到正确的函数指针类型。

可以看到标准输出中，检测结果都是 false，因为 Derived 类不可能 ReturnType (Fallback::*)(Args...) 这个函数，永远
无法匹配到返回 No& 的 func(Check<U, &U::func_name>*)
@stdout:
A has function display(int):  true
B has function display:  true
C has function display:  true
D has function display:  true
E has function display:  true
 */
namespace detecting_function {
namespace bad_case {
#define GENERATE_HAS_FUNCTION(func_name)                                           \
  template <class T, class ReturnType, class... Args>                              \
  class HasFunction_##func_name {                                                  \
    struct Fallback {                                                              \
      ReturnType func_name(Args...);                                               \
    };                                                                             \
    struct Derived : T, Fallback {};                                               \
    template <class U, ReturnType (U::*)(Args...)>                                 \
    struct Check {};                                                               \
    using NO = char[1];                                                            \
    using YES = char[2];                                                           \
    template <class U>                                                             \
    static NO& func(Check<U, &U::func_name>*);                                     \
    template <class U>                                                             \
    static YES& func(...);                                                         \
                                                                                   \
  public:                                                                          \
    static constexpr bool value = (sizeof(func<Derived>(nullptr)) == sizeof(YES)); \
  };                                                                               \
  template <class T, class ReturnType, class... Args>                              \
  constexpr bool has_function_##func_name =                                        \
      HasFunction_##func_name<T, ReturnType, Args...>::value;

GENERATE_HAS_FUNCTION(display);
}  // namespace bad_case

namespace solution {
#define GENERATE_HAS_FUNCTION_2(func_name)                                   \
  template <class T, class ReturnType, class... Args>                        \
  class HasFunction_##func_name {                                            \
    template <class U, ReturnType (U::*)(Args...)>                           \
    struct Check {};                                                         \
    using NO = char[1];                                                      \
    using YES = char[2];                                                     \
    template <class U>                                                       \
    static YES& func(Check<U, &U::func_name>*);                              \
    template <class U>                                                       \
    static NO& func(...);                                                    \
                                                                             \
  public:                                                                    \
    static constexpr bool value = (sizeof(func<T>(nullptr)) == sizeof(YES)); \
  };                                                                         \
  template <class T, class ReturnType, class... Args>                        \
  constexpr bool has_function_##func_name =                                  \
      HasFunction_##func_name<T, ReturnType, Args...>::value;

GENERATE_HAS_FUNCTION_2(display);
}  // namespace solution

struct A {
  void display(int) {}
};

struct B {
  void display() {}
};

struct C : A, B {};

struct D {
  int display(int) { return 0; }
};

struct E {};

void run() {
  std::println();
  {
    using namespace detecting_function::bad_case;
    std::println("A has function display(int):  {}", has_function_display<A, void, int>);
    std::println("B has function display:  {}", has_function_display<B, void, int>);
    std::println("C has function display:  {}", has_function_display<C, void, int>);
    std::println("D has function display:  {}", has_function_display<D, void, int>);
    std::println("E has function display:  {}", has_function_display<E, void, int>);
  }
  std::println();
  {
    using namespace detecting_function::solution;
    std::println("A has function display(int):  {}", has_function_display<A, void, int>);
    std::println("B has function display:  {}", has_function_display<B, void, int>);
    std::println("C has function display:  {}", has_function_display<C, void, int>);
    std::println("D has function display:  {}", has_function_display<D, void, int>);
    std::println("E has function display:  {}", has_function_display<E, void, int>);
  }
}
}  // namespace detecting_function


// c++17 decltype
namespace cpp17 {
template <class T, class = void>
constexpr bool has_X = false;
template <class T>
constexpr bool has_X<T, std::void_t<decltype(&T::X)>> = true;

struct A {
  int X;
};  // struct A
struct B {
  void X();
};  // struct B
struct C {
};  // struct C
void run() {
  std::println("A has X: {}", has_X<A>);
  std::println("B has X: {}", has_X<B>);
  std::println("C has X: {}", has_X<C>);
}
}  // namespace cpp17

// c++20 requires
// namespace detecting_member 和 namespace cpp11 都只能检测是否有一个成员变量或成员函数叫 X，通过 c++20 的
// requires 可以具体到是否有一个类成员叫 X (T.X)
namespace cpp20 {
template <class T>
constexpr bool has_X = requires(T) {
  &T::X;
};
template <class T>
constexpr bool has_X_member = requires(T t) {
  t.X;
};

struct A {
  int X;
};  // struct A
struct B {
  void X();
};  // struct B
struct C {
};  // struct B
void run() {
  std::println("A has X member: {}", has_X_member<A>);
  std::println("B has X member: {}", has_X_member<B>);
  std::println("C has X member: {}", has_X_member<C>);
}
}  // namespace cpp20

int main() {
  solution::run();
  detecting_member::run();
  detecting_type::run();
  detecting_function::run();
  cpp17::run();
  cpp20::run();

  return 0;
}
