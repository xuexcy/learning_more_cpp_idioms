/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/02/26 20:08:25
# Desc   :
########################################################################
为了让模板类使用固定的类型名称，并且在特化中也生效，可能通过 traits 的方法来实现。
主要点在于:
  1. 定义 concept & requires
  2. 在模板类中使用 concept & requires 以及 using 语法
  3. 另外，也可以继承多个 traits 类(这些 traits 类的 size 都是 0)
*/

#include <cstddef>
#include <iterator>
#include "utils/main_decorator.h"
namespace detail {
template <class C>
concept container = requires {
  typename C::value_type;
  typename C::sizes_type;
  typename C::difference_type;
  typename C::iterator;
  typename C::const_iterator;
  typename C::reference;
  typename C::const_reference;
};  // concept container

template <class C>
struct container_mixin {};  // struct container_mixin

template <container C>
struct container_mixin<C> {
  using value_type = typename C::value_type;
  using sizes_type = typename C::sizes_type;
  using difference_type = typename C::difference_type;
  using iterator = typename C::iterator;
  using const_iterator = typename C::const_iterator;
  using reference = typename C::reference;
  using const_reference = typename C::const_reference;
};  // struct container_mixin<C>

template <class C>
struct allocator_mixin {};  // struct allocator_mixin
template <container C>
requires requires { typename C::allocator_type; }
struct allocator_mixin<C> {
  using allocator_type = typename C::allocator_type;
};  // struct allocator_mixin<C>

template <class C>
struct reverse_iterator_mixin {
};  // struct reverse_iterator_mixin
template <container C>
requires requires {
  typename C::reverse_iterator;
  typename C::const_reverse_iterator;
}
struct reverse_iterator_mixin<C> {
  using reverse_iterator = typename C::reverse_iterator;
  using const_reverse_iterator = typename C::const_reverse_iterator;
};  // struct reverse_iterator_mixin<C>

template <class C>
struct pointer_mixin {};  // struct pointer_mixin
template <container C>
requires requires {
  typename C::pointer;
  typename C::const_pointer;
}
struct pointer_mixin<C> {
  using pointer = typename C::pointer;
  using const_pointer = typename C::const_pointer;
};  // struct pointer_mixin<C>

}  // namespace detail


template <class C>
struct container_traits :
  detail::container_mixin<C>,
  detail::allocator_mixin<C>,
  detail::reverse_iterator_mixin<C>,
  detail::pointer_mixin<C> {
};  // struct container_traits

template <class T, std::size_t N>
struct container_traits<T[N]> {
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T&;
  using const_reference = const T&;
  using pointer = T*;
  using const_pointer = const T*;
  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
};  // struct container_traits<T[N]>

int main() {
  utils::MainDecorator::Access();
  return 0;
}
