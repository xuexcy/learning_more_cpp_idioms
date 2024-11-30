/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/27 22:37:45
# Desc    :
########################################################################
*/
#pragma once

#include <array>
#include <vector>

namespace test_array {
const size_t N = 1000;
const size_t PRIME_1 = 31;
const size_t PRIME_2 = 37;

using StdArray = std::array<double, N>;
using StdVector = std::vector<double>;

template <class T, class U>
void init_array(T& t, U& u) {
    for (auto i = 0; i < t.size(); ++i) {
        t[i] = i % PRIME_1;
        u[i] = i % PRIME_2;
    }
}

const double EPSILON{0.000001};

}  // namespace test_array
