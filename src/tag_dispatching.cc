/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/01 16:33:45
# Desc   :
########################################################################
tag dispatching: 标签调度,在编译期通过类型确定调用的重载函数
通过 enable_if 也能确定调用的重载函数，但是无法保证其他的函数不符合条件，也就是可能有多个重载函数的
enable_if 都是 true 通过 tag dispatching
可以保证只有一个重载函数被调用，因为精确匹配优先级高于基类匹配
*/

#include <algorithm>
#include <format>
#include <numeric>
#include <print>
#include <string>
#include <utility>
#include <vector>

template <class Iterator>
std::string to_string(Iterator begin, Iterator end) {
  return std::format("{}", std::accumulate(begin, end, std::string{},
      [](const std::string& pre, const auto& x) {
        return std::format("{}{}{}", pre, (pre.empty() ? "" : ", "),  std::to_string(x));
      }));
}

namespace detail {
// 精确匹配比基类更好
struct Pick_3 {};                  // struct Pick_3
struct Pick_2 : public Pick_3 {};  // struct Pick_2
struct Pick_1 : public Pick_2 {};  // struct Pick_1
static constexpr Pick_1 kSelector{};

/*
1. 如果容器有 remove_if 方法，调用 remove_if 方法
2. 如果容器没有 remove_if 方法，调用 std::remove_if 方法，但是 remove_if
只会将不满足条件的元素移到前面，不会删除元素，所以需要调用 erase 方法删除元素 所以还需要调用 erase
方法
3. 如果不能对容器使用 std::remove_if 或者 std::erase 方法，那么就通过遍历容器的方式来实现
如果在三个函数中通过 enable_if 来实现，那么满足第一个要求的，很有可能满足剩下的两个函数的 enable_if
条件，这样就会造成 函数调用的 ambiguous‌，通过 tag dispatching 的方法来实现的话，就能保证重载函数调用的优先级，
即使不需要优先级，也可以使用这种方法来确定调用那个函数，比如 iterator_category
*/
template <typename Container, typename Op>
auto remove_if(Pick_1, Container& cont, Op&& op)
    -> decltype(cont.remove_if(std::forward<Op>(op)), void()) {
  std::println("container's remove_if");
  cont.remove_if(std::forward<Op>(op));
}

template <typename Container, typename Op>
auto remove_if(Pick_2, Container& cont, Op&& op) -> decltype(cont.erase(
    std::remove_if(std::begin(cont), std::end(cont), std::forward<Op>(op)), std::end(cont)),
    void()) {
  std::println("std::remove_if and std::erase");
  cont.erase(
      std::remove_if(std::begin(cont), std::end(cont), std::forward<Op>(op)), std::end(cont));
}

//template <typename Container, typename Op>
//void remove_if(Pick_3, Container& cont, Op&& op) {
//  std::println("loop remove_if");
//  for (auto it = std::begin(cont), end = std::end(cont); it != end;) {
//    if (std::invoke_r<bool>(std::forward<Op>(op), *it)) {
//      it = cont.erase(it);
//    } else {
//      ++it;
//    }
//  }
//}
}  // namespace detail

template <typename Container, typename Op>
void remove_if(Container& cont, Op&& op) {
  detail::remove_if(detail::kSelector, cont, std::forward<Op>(op));
}


// 类似的，在 int_to_type 中的 Int2Type 类型也有差不多的功能，都是在编译期通过条件来确定类型

class MyClassWithRemoveIf {
public:
  MyClassWithRemoveIf(std::initializer_list<int> il) : vec_(il) {}
  // 只对原始下在 index % 2 == 0 的时候 remove
  template <class Op>
  void remove_if(Op&& op) {
    for (int primitive_idx = 0, cur_idx = 0; primitive_idx < vec_.size(); ++primitive_idx) {
      if (0 == primitive_idx % 2 && std::invoke_r<bool>(std::forward<Op>(op), vec_[cur_idx])) {
        vec_.erase(vec_.begin() + cur_idx);
      } else {
        ++cur_idx;
      }
    }
  }
  std::string to_string() {
    return ::to_string(vec_.begin(), vec_.end());
  }
private:
  std::vector<int> vec_;
};  // class MyClassWithRemoveIf
void run() {
  // index % 2 == 0 && elem % 3 == 0
  MyClassWithRemoveIf my_class{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  remove_if(my_class, [](auto elem) { return elem % 3 == 0; });  // 1, 2, 3, 4, 5, 7, 8, 9
  std::println("my_class after remove_if: {}\n", my_class.to_string());

  // elem % 3 == 0
  std::vector<int> vec{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  remove_if(vec, [](auto elem) { return elem % 3 == 0; });  // 1, 2, 4, 5, 7, 8
  std::println("vec after remove_if: {}\n", ::to_string(vec.begin(), vec.end()));
}

// 在 std::advance 函数中，可以根据不同的 iterator_tag 来确定如何哪一个 __advance 函数，比如 forward_iterator
// 的实现就是 while (--n) ++iter，而 random_access_iterator 的实现就是 iter += n
int main() {
  run();
  return 0;
}
