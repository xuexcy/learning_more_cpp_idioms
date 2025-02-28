/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/02/27 15:59:33
# Desc   :
########################################################################
在 c++ 11 以前，enum 是定义在当前 namespace 的整型(比如 unsigned int, int), 例如
enum color { red, green, blue }; 则等价于
unsigned int red = 0;
unsigned int green = 1;
unsigned int blue = 2;
parameterized_base_class.cc
如此则存在一些问题:
1. 不同的 enum 之间可以比较，实际上有时我们不应该直接比较两个 enum
2. 在相同的 namespace 下，不能有两个同名的枚举值名称，比如 color::red 和 RGB::red

@solution:
1. 为每一个 enum 提供一个 'namespace'，这里我们将 enum 封装到一个 struct 中，这样就不会出现同名冲突
2. 通过 parameterized_base_class 将这个 struct 的枚举特性继承到 struct safe_enum 中:
  a. struct safe_enum 提供构造函数，将 enum 封装到 safe_num::val 中，这样这个构造出来的实例就可以代表枚举值，
    并且类型是 safe_num<T>，两个不同的 safe_num<T> 也就不能直接比较。而直接使用 enum 的话，两个不同的枚举是可以
    当成整型进行比较的
  b. struct safe_enum 提供比较操作符 operator==、operator!= 等

Note: 在 c++ 11 中可以使用 enum class


*/


#include <algorithm>
#include "utils/main_decorator.h"
#include "cpp_utils/util.h"

namespace unsafe_enum {
enum color { red, green, blue };
enum shape { circle, square, triangle };
// enum RGB { red, green, blue };  // 编译错误: RGB::red 和 color::red 重名

void run() {
  PRINT_CURRENT_FUNCTION_NAME;
  color c = red;
  // 编译器提示:
  // Comparison of different enumeration types ('color' and 'unsafe_enum::shape') is deprecated
  // clang(-Wdeprecated-enum-compare)
  bool flag = (c >= triangle);  // 这里两个枚举值可以当成整型直接比较
  std::println(
    "sizeof(enum color): {}, underlying type: {}",
    sizeof(color), typeid(std::underlying_type<color>).name());
  std::println("color::red >= shape::triangle: {}", flag);
}
}  // namespace unsafe_enum

namespace type_safe_enum {
template <class def, class inner = def::type>
class safe_enum : public def {
  using type = inner;
  inner val;
  static safe_enum array[def::end_ - def::begin_];
  static bool init;

  static void initialize() {
    if (!init) {
      unsigned int size = def::end_ - def::begin_;
      for (unsigned int i = 0, j = def::begin_; i < size; ++i, ++j) {
        array[i] = static_cast<def::type>(j);
      }
      init = true;
    }
  }
public:
  safe_enum() = default;
  safe_enum(type v) : val(v) {}
  type underlying() const { return val; }

  static safe_enum* begin() {
    initialize();
    return array;
  }
  static safe_enum* end() {
    initialize();
    return array + (def::end_ - def::begin_);
  }

  friend bool operator==(const safe_enum& lhs, const safe_enum& rhs) {
    return lhs.val == rhs.val;
  }
  friend bool operator!=(const safe_enum& lhs, const safe_enum& rhs) {
    return lhs.val != rhs.val;
  }
  friend bool operator<(const safe_enum& lhs, const safe_enum& rhs) {
    return lhs.val < rhs.val;
  }
  friend bool operator<=(const safe_enum& lhs, const safe_enum& rhs) {
    return lhs.val <= rhs.val;
  }
  friend bool operator>(const safe_enum& lhs, const safe_enum& rhs) {
    return lhs.val < rhs.val;
  }
  friend bool operator>=(const safe_enum& lhs, const safe_enum& rhs) {
    return lhs.val <= rhs.val;
  }
};  // class safe_enum

template <class def, class inner>
safe_enum<def, inner> safe_enum<def, inner>::array[def::end_ - def::begin_];

template <class def, class inner>
bool safe_enum<def, inner>::init = false;


struct color_def {
  enum type { red, green, blue };
  const static size_t begin_ = type::red;
  const static size_t end_ = type::blue + 1;
};  // struct color_def
using color = safe_enum<color_def>;

struct shape_def {
  enum type { circle, square, triangle };
  const static size_t begin_ = type::circle;
  const static size_t end_ = type::triangle + 1;
};  // struct shape_def
using shape = safe_enum<shape_def, unsigned char>;

template <class Enum>
void f(Enum e) {
  std::println("Enum value: {}", e.underlying());
  std::for_each(color::begin(), color::end(), &f<color>);
}
void run() {
  PRINT_CURRENT_FUNCTION_NAME;
  color c = color::red;
  // bool flag = (c >= shape::triangle);  // 编译错误

  color c2 = color::green;
  bool flag2 = (c == c2);

  bool flag3 = (c == color::red);  // color::red 会调用构造函数，即 color(color::red)
}
}  // namespace type_safe_enum


int main() {
  utils::MainDecorator::Access();
  unsafe_enum::run();
  type_safe_enum::run();
  return 0;
}


