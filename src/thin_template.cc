/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/10 20:35:53
# Desc   :
########################################################################
对于模板类，编译器会在代码使用该类时进行实例化，比如使用了 vector<int>, vector<float>, vector<double>,
那么就是实例化三份 vector 的代码，如果实例化大量的模板类，就会导致编译出的目标代码很大。

@solution: 当模板类中有一些和模板参数无关的成员或函数时，可以将这些共用的代码提取到一个 Base 中，再由模板类继承，这样
就可以减少每个模板类的目标代码大小，而且重用相同的目标代码还可以提高指令缓存性能，从而提高应用程序性能
*/

#include <cstddef>
#include <format>
#include <numeric>
#include <string>

#include "cpp_utils/util.h"
#include "utils/main_decorator.h"


// 对于 Vector 来说，size_, capacity_ 以及数据存储的地址 data_，都是和模板参数类型 T 无关的
// 按照 bad_case::Vector 的实现，每个实例化的 Vector 的目标代码中，都有这三个成员变量
namespace bad_case {
template <class T>
class Vector {
public:
  // 参考 generic_container.cc solution::Stack
  Vector(size_t capacity = 1024)
      : capacity_(capacity), data_(static_cast<T*>(::operator new(sizeof(T) * capacity_))) {}
  void push_back(const T& value) {
    if (size_ == capacity_) {
      return;
    }
    new (data_ + size_) T(value);
    ++size_;
  }
  T& operator[](size_t index) { return data_[index]; }
  size_t size() const { return size_; }
  std::string to_string() const {
    std::string init;
    return std::accumulate(data_, data_ + size_, std::string{}, [](const std::string& pre, const T& cur) {
      return pre.empty() ? std::format("{}", cur) : std::format("{}, {}", pre, cur);
    });
  }

private:
  size_t capacity_{0};
  size_t size_{0};
  T* data_{nullptr};
};  // class Vector

void run() {
  PRINT_CURRENT_FUNCTION_NAME;
  Vector<int> v1;
  v1.push_back(1);
  v1.push_back(2);
  Vector<double> v2;
  v2.push_back(1.0f);
  v2.push_back(2.0f);
  Vector<float> v3;
  v3.push_back(1.0);
  v3.push_back(2.0);

  std::println("{}", v1.to_string());
  std::println("{}", v2.to_string());
  std::println("{}", v3.to_string());
}
}  // namespace bad_case

// 通过将和模板参数无关的成员放到 VectorBase 中，这样在 Vector 实例化时，这些 Vector 就可以共用 VectorBase 的
// 目标代码。另外，当 size_ == capacity_ 后， Vector 可能还要申请更多的内存，那么申请内存的函数也可以放到
// VectorBase 中，并定义为 void grow(size_t mem_size); ，这样就不用在每个实例化的 Vector 中存储一份 grow 函数
// 的代码。如此，便可以减少模板代码大小
namespace solution {
class VectorBase {
public:
  VectorBase(size_t capacity_, size_t obj_size)
      : capacity_(capacity_), data_(::operator new(capacity_ * obj_size)) {}
  size_t size() const { return size_; }

protected:
  size_t capacity_{0};
  size_t size_{0};
  void* data_{nullptr};
};  // class VectorBase

template <class T>
class Vector : private VectorBase {
public:
  Vector(size_t capacity = 1024) : VectorBase(capacity, sizeof(T)) {}
  void push_back(const T& value) {
    if (size_ == capacity_) {
      return;
    }
    new (static_cast<T*>(data_) + size_) T(value);
    ++size_;
  }
  T& operator[](size_t index) { return static_cast<T*>(data_)[index]; }
  std::string to_string() const {
    T* data = static_cast<T*>(data_);
    return std::accumulate(
        data, data + size_, std::string{}, [](const std::string& pre, const T& cur) {
          return pre.empty() ? std::format("{}", cur) : std::format("{}, {}", pre, cur);
        });
  }
};  // class Vector

void run() {
  PRINT_CURRENT_FUNCTION_NAME;
  Vector<int> v1;
  v1.push_back(1);
  v1.push_back(2);
  Vector<double> v2;
  v2.push_back(1.0f);
  v2.push_back(2.0f);
  Vector<float> v3;
  v3.push_back(1.0);
  v3.push_back(2.0);

  std::println("{}", v1.to_string());
  std::println("{}", v2.to_string());
  std::println("{}", v3.to_string());
}
}  // namespace solution

int main() {
  utils::MainDecorator::Access();
  bad_case::run();
  solution::run();
  return 0;
}
