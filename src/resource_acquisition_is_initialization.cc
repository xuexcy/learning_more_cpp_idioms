/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/09 15:19:45
# Desc   :
########################################################################
1. 申请的内存资源忘记 delete
2. 锁忘记 release

@solution:
将资源和锁封装到模板类，通过模板类的析构函数来释放资源或解锁
*/

#include <mutex>
#include <print>
#include "utils/main_decorator.h"
namespace bad_case {

void foo() {
  bool flag_1{true};
  bool flag_2{false};
  char* ch = new char[100];
  if (flag_1) {
    std::println("flag_1 true");
    if (flag_2) {
      std::println("flag_2 true");
    } else {
      std::println("flag_2 false");
      return;
    }
  } else {
    throw "ERROR";
  }
  delete [] ch;
}

void run() {
  foo();
}
}  // namespace bad_case

namespace solution {
class NonCopyable {
public:
  NonCopyable() = default;
private:
  NonCopyable(const NonCopyable&);
  NonCopyable& operator=(const NonCopyable&);
};  // class NonCopyable

// 自动删除资源
template <class T>
class AutoDelete : NonCopyable {
public:
  AutoDelete(T* p = nullptr) : ptr_(p) {}
  ~AutoDelete() noexcept {
    std::println("auto delete: {}", (void*)ptr_);
    delete ptr_;
  }
private:
  T* ptr_;
};  // class AutoDelete

template <class Mutex>
class ScopedLock : NonCopyable {
public:
  ScopedLock(Mutex& m): mtx_(m) {
    std::println("lock mutex");
    mtx_.lock();
  }
  ~ScopedLock() noexcept {
    std::println("unlock mutex");
    mtx_.unlock();
  }
private:
  Mutex& mtx_;
};  // class ScopedLock

void run() {
  AutoDelete<int> safe_del(new int(8));
  std::mutex m;
  ScopedLock<std::mutex> lock(m);
}
}  // namespace solution

int main() {
  utils::MainDecorator::Access();
  bad_case::run();
  solution::run();
  return 0;
}

