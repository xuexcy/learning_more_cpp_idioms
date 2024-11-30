/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/29 11:30:15
# Desc    :
########################################################################
*/

#include "gtest/gtest.h"

#include <type_traits>

#include "expression_template_deps/test_array/calculation_test_deps.h"


TEST(CalculationTest, CheckEqual) {
    StdArray arr_a_data, arr_b_data;
    std::vector<double> vec_a_data(N), vec_b_data(N);
    init_array(arr_a_data, arr_b_data);
    init_array(vec_a_data, vec_b_data);

    using namespace array_operator_overload;
    StdArray res1 = calculate(arr_a_data, arr_b_data);
    StdArray res2 = calculate_by_expression(arr_a_data, arr_b_data);

    using namespace array_expression_template;
    Array arr_a(arr_a_data), arr_b(arr_b_data);
    Array vec_a(vec_a_data), vec_b(vec_b_data);

    #define SAME_TYPE(a, b, res) \
        std::is_same_v< ExprType<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>>, std::decay_t<decltype(res)>>
    // arr arr
    auto res3 = calculate_by_expression_template(arr_a, arr_b);
    static_assert(SAME_TYPE(arr_a, arr_b, res3));
    // vec vec
    auto res4 = calculate_by_expression_template(vec_a, vec_b);
    static_assert(SAME_TYPE(vec_a, vec_b, res4));
    // arr vec
    auto res5 = calculate_by_expression_template(arr_a, vec_b);
    static_assert(SAME_TYPE(arr_a, vec_b, res5));
    // vec arr
    auto res6 = calculate_by_expression_template(vec_a, arr_b);
    static_assert(SAME_TYPE(vec_a, arr_b, res6));

    for (auto i = 0; i < res2.size(); ++i) {
        ASSERT_NEAR(res1[i], res2[i], EPSILON);
        ASSERT_NEAR(res1[i], res3[i], EPSILON);
        ASSERT_NEAR(res1[i], res4[i], EPSILON);
        ASSERT_NEAR(res1[i], res5[i], EPSILON);
        ASSERT_NEAR(res1[i], res6[i], EPSILON);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
