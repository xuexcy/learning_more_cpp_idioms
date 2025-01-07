/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/06 23:23:16
# Desc   :
########################################################################
使用 throw e 抛出异常，会对异常 e 进行拷贝构造，如果 e 的类型是一个 Base类，那么通过 throw e 的方式
就会丢失 Derived 的信息

@solution: 在需要抛出异常时，不直接使用 throw，而是通过调用虚函数的方式来实现，在虚函数中 throw *this，
这样就不会丢弃 Derived 的信息


另外，如果需要重新抛出异常，可以直接 throw，因为 throw e 会发生一次拷贝构造
*/

#include <exception>
#include <print>

#include "cpp_utils/util.h"

#define PRINT_CONSTRUCTOR \
  PRINT_CURRENT_FUNCTION_NAME; \
  std::println("addr: {}", (void*)(this));

namespace bad_case {
struct ExceptionBase : public std::exception {
  ExceptionBase() {
    PRINT_CONSTRUCTOR;
  }
  ExceptionBase(const ExceptionBase&) {
    PRINT_CONSTRUCTOR;
  }
};  // struct ExceptionBase
struct ExceptionDerived : public ExceptionBase {
  ExceptionDerived() {
    PRINT_CONSTRUCTOR;
  }
  ExceptionDerived(const ExceptionDerived& e): ExceptionBase(e) {
    PRINT_CONSTRUCTOR;
  }
};  // struct ExceptionDerived

void foo(ExceptionBase& e) {
  PRINT_CURRENT_FUNCTION_NAME;
  // 这里 throw e 是静态类型 ExceptionBase
  throw e;
}

/**
根据 stdout 我们可以看到
1. ExceptionDerived e: 发生了 ExceptionBase 和 ExceptionDerived 的构造
2. 在 foo(e) 中的 throw e: 发生了对 ExceptionBase e 的拷贝构造
3. 在 try catch 中的 ExceptionDerived e 无法接住 foo 抛出来的 ExceptionBase 类型的异常
4. 异常由 std::exception 接住
*/
void run() {
  PRINT_CURRENT_FUNCTION_NAME;
  ExceptionDerived e;
  try {
    foo(e);
  } catch (ExceptionDerived& e) {
    // 对于 foo 函数抛出的 ExceptionBase，这里接不住
    std::println("ExceptionDerived, addr: {}", (void*)(&e));
  } catch (std::exception& e) {
    std::println("std::exception, addr: {}", (void*)(&e));
  }
}
}  // namespace bad_case

namespace solution {
struct ExceptionBase : public std::exception {
  ExceptionBase() {
    PRINT_CONSTRUCTOR;
  }
  ExceptionBase(const ExceptionBase&) {
    PRINT_CONSTRUCTOR;
  }
  virtual void raise() { throw *this; }
  virtual ~ExceptionBase() {}
};  // struct ExceptionBase

struct ExceptionDerived : public ExceptionBase {
  ExceptionDerived() {
    PRINT_CONSTRUCTOR;
  }
  ExceptionDerived(const ExceptionDerived& e): ExceptionBase(e) {
    PRINT_CONSTRUCTOR;
  }
  virtual void raise() override { throw *this; }
  virtual ~ExceptionDerived() {}
};  // struct ExceptionDerived

void foo(ExceptionBase& e) {
  PRINT_CURRENT_FUNCTION_NAME;
  // 通过 e.raise() 是利用了虚函数，是一个动态类型
  e.raise();
}


/**
根据 stdout 我们可以看到
1. ExceptionDerived e: 发生了 ExceptionBase 和 ExceptionDerived 的构造
2. 在 foo(e) 中的 e.raise()，即 throw e: 因为多态的原因, 发生了对 ExceptionDerived e 的拷贝构造
3. 在 try catch 中的 ExceptionDerived e 接住了由 foo 中的 e.erase() 抛出来的 ExceptionDerived 类型的异常
*/
void run() {
  PRINT_CURRENT_FUNCTION_NAME;
  ExceptionDerived e;
  try {
    foo(e);
  } catch (ExceptionDerived& e) {
    std::println("ExceptionDerived, addr: {}", (void*)(&e));
  } catch (std::exception& e) {
    std::println("there is an exception, addr: {}", (void*)(&e));
  }

}

} // namespace solution


namespace rethrow {
using namespace solution;

// bad: 通过 throw e 的方式，没有使用多态
void run_rethrow_1() {
  PRINT_CURRENT_FUNCTION_NAME;
  ExceptionDerived e;
  try {
    foo(e);
  } catch (ExceptionBase& e) {
    std::println("ExceptionBase, rethrow, addr: {}", (void*)(&e));
    // 这里会将 e 拷贝一份，这样 throw 的类型就是 ExceptionBase，导致在 Derived 中的异常信息丢失
    throw e;
  } catch (std::exception& e) {
    std::println("there is an exception, addr: {}", (void*)(&e));
  }
}
// good: 通过 throw 将原来的异常重新抛出
void run_rethrow_2() {
  PRINT_CURRENT_FUNCTION_NAME;
  ExceptionDerived e;
  try {
    foo(e);
  } catch (ExceptionBase& e) {
    std::println("ExceptionBase, rethrow, addr: {}", (void*)(&e));
    // 直接 throw，将原来的 e 重新抛出
    throw;
  } catch (std::exception& e) {
    std::println("there is an exception, addr: {}", (void*)(&e));
  }
}
// not suggested: 在 e.raise() 的时候，在内部 throw *this 时又发生了一次拷贝 构造
void run_rethrow_3() {
  PRINT_CURRENT_FUNCTION_NAME;
  ExceptionDerived e;
  try {
    foo(e);
  } catch (ExceptionBase& e) {
    std::println("ExceptionBase, rethrow, addr: {}", (void*)(&e));
    // 使用 e 再 throw 一次，抛出的是一个 e 的拷贝(不建议这么用)
    e.raise();
  } catch (std::exception& e) {
    std::println("there is an exception, addr: {}", (void*)(&e));
  }
}
void catch_rethrow(void (*func)()) {
  PRINT_CURRENT_FUNCTION_NAME;
  try {
    func();
  } catch (ExceptionDerived& e) {
    std::println("ExceptionBase, rethrow, addr: {}", (void*)(&e));
  } catch (std::exception& e) {
    std::println("there is an exception, addr: {}", (void*)(&e));
  }
}

/**
根据 stdout，我们可以看到
- rethrow_1:
  1. 在被 ExceptionBase& e 接住异常后，又通过 throw e 抛出了异常，此类型和 bad_case 类型，发生了对 ExceptionBase e 的拷贝
  2. 在 catch_rethrow 中的 ExceptionDerived& e 无法接住异常
  3. 在 catch_rethrow 中的 std::exception& 接住异常
- rethrow_2:
  1. 在被 ExceptionBase& e 接住异常后，又通过 throw 抛出了异常，此时没有发生拷贝构造，而是将原来的 e 重新抛出
  2. 在 catch_rethrow 中的 ExceptionDerived& e 接住异常，可以看到，重新抛出的异常的实例的地址没有发生变化
- rethrow_3:
  1. 在被 ExceptionBase& e 接住异常后，又通过 e.raise() 抛出了异常，此时 e.raise() 中的 throw e 对 ExceptionDerived e 进行了拷贝构造
  2. 在 catch_rethrow 中的 ExceptionDerived& e 接住异常，可以看到，重新抛出的异常的实例的地址发生了变化
 */
void run() {
  // bad
  catch_rethrow(run_rethrow_1);
  std::println();
  // good
  catch_rethrow(run_rethrow_2);
  std::println();
  // not suggested
  catch_rethrow(run_rethrow_3);
}

}  // namespace rethrow


int main() {
  bad_case::run();
  std::println();
  solution::run();
  std::println();
  rethrow::run();
  return 0;
}
