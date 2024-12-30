/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2024/12/30 20:31:24
# Desc   :
########################################################################
模板层次结构: 通过模板展开单一继承，我们可以实现一个类的多重继承
*/

#include <print>
#include "cpp_utils/util.h"
#include "utils/main_decorator.h"

namespace multiple_inheritance {
struct DanceA {
  void dance() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct DanceB {
  void dance() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct DanceC {
  void dance() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct JokeA {
  void joke() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct JokeB {
  void joke() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct JokeC {
  void joke() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct SongA {
  void sing() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct SongB {
  void sing() { PRINT_CURRENT_FUNCTION_NAME; }
};

struct SongC {
  void sing() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class... Ts>
struct GenHierarchy : Ts... {};

}  // namespace multiple_inheritance

namespace template_hierarchy {
template <template <class T> class... _PolicyTs>
struct GenHierarchy;

template <template <class T> class _HeadPolicyT, template <class T> class... _TailPolicyTs>
struct GenHierarchy<_HeadPolicyT, _TailPolicyTs...> : _HeadPolicyT<GenHierarchy<_TailPolicyTs...>> {
};

// 终止
template <>
struct GenHierarchy<> {};

template <class ParentT>
struct DanceA : ParentT {
  void dance() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct DanceB : ParentT {
  void dance() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct DanceC : ParentT {
  void dance() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct JokeA : ParentT {
  void joke() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct JokeB : ParentT {
  void joke() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct JokeC : ParentT {
  void joke() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct SongA : ParentT {
  void sing() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct SongB : ParentT {
  void sing() { PRINT_CURRENT_FUNCTION_NAME; }
};

template <class ParentT>
struct SongC : ParentT {
  void sing() { PRINT_CURRENT_FUNCTION_NAME; }
};
} //namespace template_hierarchy

template <class T>
void entertain(T t) {
  t.sing();
  t.dance();
  t.joke();
}

void run() {
  {
    using namespace multiple_inheritance;
    using X = GenHierarchy<DanceA, JokeB, SongC> ;
    using Y = GenHierarchy<DanceB, JokeA, SongA> ;
    using Z = GenHierarchy<DanceC, JokeC, SongB> ;
    entertain(X());
    entertain(Y());
    entertain(Z());
  }
  {
    using namespace template_hierarchy;
    using X = GenHierarchy<DanceA, JokeB, SongC> ;
    using Y = GenHierarchy<DanceB, JokeA, SongA> ;
    using Z = GenHierarchy<DanceC, JokeC, SongB> ;
    entertain(X());
    entertain(Y());
    entertain(Z());
  }
}

int main() {
  utils::MainDecorator::Access();
  run();
  return 0;
}
