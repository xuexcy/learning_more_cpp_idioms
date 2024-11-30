/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/26 12:16:00
# Desc    :
########################################################################
*/
#pragma once

#include <cassert>

#include "concepts.h"

namespace array_operator_overload {
template <class Array>
Array get_array(size_t n) {
    if constexpr (has_construct_with_size<Array>::value) {
        return Array(n);
    } else {
        return Array();
    }
}
template <ArrayLike Array>
Array operator+(const Array& a, const Array& b) {
    assert(a.size() == b.size());
    Array result = get_array<Array>(b.size());
    for (auto i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

template <class T, ArrayLike Array>
requires ItemOf<T, Array>
Array operator+(const T& a, const Array& b) {
    Array result = get_array<Array>(b.size());
    assert(b.size() == result.size());
    for (auto i = 0; i < b.size(); ++i) {
        result[i] = a + b[i];
    }
    return result;
}
template <ArrayLike Array, class T>
requires ItemOf<T, Array>
Array operator+(const Array& a, const T& b) {
    return b + a;
}


template <ArrayLike Array>
Array operator*(const Array& a, const Array& b) {
    assert(a.size() == b.size());
    Array result = get_array<Array>(b.size());
    assert(b.size() == result.size());
    for (auto i = 0; i < a.size(); ++i) {
        result[i] = a[i] * b[i];
    }
    return result;
}

template <class T, ArrayLike Array>
requires ItemOf<T, Array>
Array operator*(const T& a, const Array& b) {
    Array result = get_array<Array>(b.size());
    assert(b.size() == result.size());
    for (auto i = 0; i < b.size(); ++i) {
        result[i] = a * b[i];
    }
    return result;
}
template <ArrayLike Array, class T>
requires ItemOf<T, Array>
Array operator*(const Array& a, const T& b) {
    return b * a;
}
}  // namespace array_operator_overload
