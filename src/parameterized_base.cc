/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/05 16:55:43
# Desc    :
########################################################################

我们抽象出一个方面/要求/能力(class Interface)，希望把它能在一个给定的类型 T 上实现。
和一般的由 T 继承 Interface 不同，这次我们希望不要改变 T 的定义，比如 T
是某个三方库中的数据结构，我们没有理由去变更 T 的定义

假设我们希望将一个类的实例序列化，我们把这个能力叫做 class ISerializable
    1. 将 T 的实例序列化到一段 buffer，即实现 class ISerializable
    2. 不修改 class T 的定义且 T 的实例仍然可以继续使用

@solution: 由一个新的类继承 T 和 ISerializable，并实现 ISerializable
要求的能力，这样这个新的类就能满足我们抽象 出的能力，又能被当成 T 使用

@注意: only if T is a user-defined POD type without pointers(基本类型不能被继承)

*/

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <format>
#include <print>
#include <stdexcept>
#include <string>
#include <vector>

#include "utils/main_decorator.h"

template <class... Args>
void reverse_container(Args&... args) {
  static_assert(sizeof...(args) > 0, "At least one argument required");
  (std::reverse(args.begin(), args.end()), ...);
}

class ISerializable {
public:
  virtual size_t serialize(char*& buffer, size_t& buf_size) const = 0;
};

// only if T is a user-defined POD type without pointers
template <class T>
class Serializable : public T, public ISerializable {
public:
  Serializable(const T& t = T()) : T(t) {}

  virtual size_t serialize(char*& buffer, size_t& buf_size) const {
    const size_t size = sizeof(T);
    if (size > buf_size) {
      throw std::runtime_error("insufficient memory!");
    }
    auto t_ptr = static_cast<const T*>(this);
    memcpy(buffer, static_cast<const T*>(this), size);
    buffer += size;
    buf_size -= size;
    return size;
  }
};

template <class T>
std::string hex_string(T t) {
  // {0:#0{1}x}
  // 以 $0 的 16 进制填充，整体宽度为 $1，不足的长度以前缀 0 填充
  return std::format("[{0:#0{1}x} = {0}]", t, sizeof(T) * 2 + 2);
}

struct Point {
  int x{0};
  short y{0};
  static const size_t SIZE;
  std::string string() const {
    return std::format("[int x = {}, short y = {}]", hex_string(x), hex_string(y), 8);
  }
};
const size_t Point::SIZE = sizeof(Point);
static_assert(8 == Point::SIZE);

struct Circle {
  Point center;
  long int radius{0};
  static const size_t SIZE;
  std::string string() const {
    return std::format(
        "[Point center = {}, long int radius = {}]", center.string(), hex_string(radius));
  }
};
const size_t Circle::SIZE = sizeof(Circle);
static_assert(16 == Circle::SIZE);

void run() {
  std::println("Point size: {}", Point::SIZE);
  std::println("Circle size: {}", Circle::SIZE);
  std::println("Serializable<Point> size: {}", sizeof(Serializable<Point>));
  std::println("Serializable<Circle> size: {}", sizeof(Serializable<Circle>));

  auto native = std::endian::native;
  std::println("machine endian: {}", native == std::endian::little ? "little" : "big");

  uint8_t data[sizeof(Point) + sizeof(Circle) + sizeof(int)];
  char* buffer = (char*)data;
  static_assert(28 == sizeof(data));
  size_t buf_size = sizeof(data);
  size_t idx = 0;

  {
    auto old_buffer = buffer;
    auto old_buf_size = buf_size;
    Point p{0x12345678, 0x0ABC};
    Serializable<Point> s_p(p);
    std::println("Serializable<Point> s_p = {}", s_p.string());

    auto used_size = s_p.serialize(buffer, buf_size);

    assert(used_size == Point::SIZE);
    assert(old_buffer + Point::SIZE == buffer);
    assert(old_buf_size == Point::SIZE + buf_size);
    // s_p.x = 0x12345678
    // s_p.y = 0x0ABC
    std::vector<uint8_t> int_x{0x78, 0x56, 0x34, 0x12};
    std::vector<uint8_t> short_y{0xBC, 0x0A};
    std::vector<uint8_t> aligning_for_y{0, 0};
    std::vector<uint8_t> aligned_y = short_y;
    aligned_y.insert(aligned_y.end(), aligning_for_y.begin(), aligning_for_y.end());

    if (native == std::endian::big) {
      reverse_container(int_x, aligned_y);
    }
    for (size_t i = 0; i < int_x.size(); ++i, ++idx) {
      if (int_x[i] == data[idx]) {
        std::println("int_x[{}] == {} == data[{}]", i, hex_string(data[idx]), idx);
      } else {
        std::println(
            "int_x[{}]: {} != data[{}]: {}", i, hex_string(int_x[i]), idx, hex_string(data[idx]));
        assert(false);
      }
    }
    for (size_t i = 0; i < aligned_y.size(); ++i, ++idx) {
      if (aligned_y[i] == data[idx]) {
        std::println("aligned_y[{}] == {} == data[{}]", i, hex_string(data[idx]), idx);
      } else {
        std::println("aligned_y[{}]: {} != data[{}]: {}", i, hex_string(aligned_y[i]), idx,
            hex_string(data[idx]));
        assert(false);
      }
    }
  }
  {
    auto old_buffer = buffer;
    auto old_buf_size = buf_size;
    Point p{0x2468ACDF, 0x3928};
    Circle c{p, 0x2825628257282931};
    Serializable<Circle> s_c(c);
    std::println("Serializable<Circle> s_c = {}", s_c.string());

    auto used_size = s_c.serialize(buffer, buf_size);

    assert(used_size == Circle::SIZE);
    assert(old_buffer + Circle::SIZE == buffer);
    assert(old_buf_size == Circle::SIZE + buf_size);
    // s_c.center.x = 0x2468ACDF
    // s_c.center.y = 0x3928
    // s_c.radius = 0x2825628257282931
    std::vector<uint8_t> int_x{0xDF, 0xAC, 0x68, 0x24};
    std::vector<uint8_t> short_y{0x28, 0x39};
    std::vector<uint8_t> aligning_for_y{0, 0};
    std::vector<uint8_t> aligned_y = short_y;
    aligned_y.insert(aligned_y.end(), aligning_for_y.begin(), aligning_for_y.end());
    std::vector<uint8_t> long_int_radius{0x31, 0x29, 0x28, 0x57, 0x82, 0x62, 0x25, 0x28};

    if (native == std::endian::big) {
      reverse_container(int_x, aligned_y, long_int_radius);
    }
    for (size_t i = 0; i < int_x.size(); ++i, ++idx) {
      if (int_x[i] == data[idx]) {
        std::println("int_x[{}] == {} == data[{}]", i, hex_string(data[idx]), idx);
      } else {
        std::println(
            "int_x[{}]: {} != data[{}]: {}", i, hex_string(int_x[i]), idx, hex_string(data[idx]));
        assert(false);
      }
    }
    for (size_t i = 0; i < aligned_y.size(); ++i, ++idx) {
      if (aligned_y[i] == data[idx]) {
        std::println("aligned_y[{}] == {} == data[{}]", i, hex_string(data[idx]), idx);
      } else {
        std::println("aligned_y[{}]: {} != data[{}]: {}", i, hex_string(aligned_y[i]), idx,
            hex_string(data[idx]));
        assert(false);
      }
    }
    for (size_t i = 0; i < long_int_radius.size(); ++i, ++idx) {
      if (long_int_radius[i] == data[idx]) {
        std::println("long_int_radius[{}] == {} == data[{}]", i, hex_string(data[idx]), idx);
      } else {
        std::println("long_int_radius[{}]: {} != data[{}]: {}", i, hex_string(long_int_radius[i]),
            idx, hex_string(data[idx]));
        assert(false);
      }
    }
  }
  // no: 基本类型不能被继承
  // {
  //     auto old_buffer = buffer;
  //     auto old_buf_size = buf_size;
  //     int num = 0x0304;
  //     Serializable<int> s_num(num);
  //     std::println("Serializable<int> s_num = {}", static_cast<int>(s_num));

  //     auto used_size = s_num.serialize(buffer, buf_size);

  //     assert(used_size == sizeof(int));
  //     assert(old_buffer + sizeof(int) == buffer);
  //     assert(old_buf_size == sizeof(int) + buf_size);

  //     std::vector<char> int_num{4, 0, 3, 0};
  //     if (native == std::endian::big) {
  //         reverse_container(int_num);
  //     }

  //     for (size_t i = 0; i < int_num.size(); ++i, ++idx) {
  //         std::println("int_num[{}]: {}, data[{}]: {}", i, int(int_num[i]), idx, int(data[idx]));
  //         assert(int_num[i] == data[idx]);
  //     }
  // }
  {
    assert(sizeof(int) == buf_size);
    assert((char*)data + Point::SIZE + Circle::SIZE == buffer);
  }
}

int main() {
  utils::MainDecorator::Access();
  run();
}
