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

template <class ArraySize>
class ArrayCalculationTest : public ::testing::Test {
protected:
    ArrayCalculationTest() : vec_a_data(ArraySize::kValue), vec_b_data(ArraySize::kValue) {}
    void SetUp() override {
        init_array(arr_a_data, arr_b_data);
        init_array(vec_a_data, vec_b_data);
    }
    StdArray<ArraySize::kValue> arr_a_data;
    StdArray<ArraySize::kValue> arr_b_data;
    StdVector vec_a_data;
    StdVector vec_b_data;
    static constexpr size_t kSize = ArraySize::kValue;
};  // class ArrayCalculationTest
TYPED_TEST_SUITE_P(ArrayCalculationTest);

TYPED_TEST_P(ArrayCalculationTest, CheckEqual) {
    constexpr size_t kSize = TypeParam::kValue;
    auto& arr_a_data = this->arr_a_data;
    auto& arr_b_data = this->arr_b_data;
    auto& vec_a_data = this->vec_a_data;
    auto& vec_b_data = this->vec_b_data;
    // by expression
    StdArray<kSize> res1 = calculate_by_expression(arr_a_data, arr_b_data);
    {
        StdVector res2 = calculate_by_expression(vec_a_data, vec_b_data);
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }

    // by overload
    {
        using namespace array_operator_overload;
        StdArray<kSize> res2 = calculate(arr_a_data, arr_b_data);
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }
    {
        using namespace array_operator_overload;
        StdVector res2 = calculate(vec_a_data, vec_b_data);
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }

    // by expression template
    using namespace array_expression_template;
    Array arr_a(arr_a_data), arr_b(arr_b_data);
    Array vec_a(vec_a_data), vec_b(vec_b_data);

    #define SAME_TYPE(a, b, res) \
        std::is_same_v< ExprType<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>>, std::decay_t<decltype(res)>>
    {
        // arr arr
        auto res2 = calculate_by_expression_template(arr_a, arr_b);
        static_assert(SAME_TYPE(arr_a, arr_b, res2));
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }
    {
        // vec vec
        auto res2 = calculate_by_expression_template(vec_a, vec_b);
        static_assert(SAME_TYPE(vec_a, vec_b, res2));
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }
    {
        // arr vec
        auto res2 = calculate_by_expression_template(arr_a, vec_b);
        static_assert(SAME_TYPE(arr_a, vec_b, res2));
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }
    {
        // vec arr
        auto res2 = calculate_by_expression_template(vec_a, arr_b);
        static_assert(SAME_TYPE(vec_a, arr_b, res2));
        for (auto i = 0; i < res2.size(); ++i) {
            ASSERT_NEAR(res1[i], res2[i], EPSILON);
        }
    }
}

REGISTER_TYPED_TEST_SUITE_P(ArrayCalculationTest, CheckEqual);
using SizeTypes = ::testing::Types<ArraySize<8>, ArraySize<32>, ArraySize<128>, ArraySize<512>, ArraySize<2048>>;
INSTANTIATE_TYPED_TEST_SUITE_P(DifferentSize, ArrayCalculationTest, SizeTypes);

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
