/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/26 22:22:01
# Desc    :
########################################################################
*/
#pragma once

#include <concepts>

template <class T>
concept ArrayLike = requires(T t, std::size_t idx) {
  { t[idx] };
  { t.size() };
};

template <ArrayLike T>
using ItemType = std::decay_t<decltype(std::declval<T>()[0])>;

template <class T, class U>
concept ItemOf = std::same_as<std::decay_t<T>, ItemType<U>>;

template <class LHS, class RHS>
concept SameItem = std::same_as<ItemType<LHS>, ItemType<RHS>>;

template <class T, class = void>
struct has_construct_with_size : public std::false_type {};

template <class T>
struct has_construct_with_size<T, std::void_t<decltype(T(size_t(1)))>> : public std::true_type {};
