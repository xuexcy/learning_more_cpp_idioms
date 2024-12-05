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

#include <type_traits>

#include "expression_template_deps/test_array/calculation_deps.h"
#include "gtest/gtest.h"

template <class ArraySize>
class ArrayCalculationTest : public ::testing::Test {
protected:
  ArrayCalculationTest()
      : vec_a_data(ArraySize::kValue),
        vec_b_data(ArraySize::kValue),
        arr_a(arr_a_data),
        arr_b(arr_b_data),
        vec_a(vec_a_data),
        vec_b(vec_b_data) {}
  void SetUp() override {
    init_array(arr_a_data, arr_b_data);
    init_array(vec_a_data, vec_b_data);
  }
  StdArray<ArraySize::kValue> arr_a_data;
  StdArray<ArraySize::kValue> arr_b_data;
  StdVector vec_a_data;
  StdVector vec_b_data;
  static constexpr size_t kSize = ArraySize::kValue;

  array_expression_template::Array<decltype(arr_a_data)> arr_a;
  array_expression_template::Array<decltype(arr_b_data)> arr_b;
  array_expression_template::Array<decltype(vec_a_data)> vec_a;
  array_expression_template::Array<decltype(vec_b_data)> vec_b;
};  // class ArrayCalculationTest
TYPED_TEST_SUITE_P(ArrayCalculationTest);

TYPED_TEST_P(ArrayCalculationTest, CheckEqual) {
  constexpr size_t kSize = TypeParam::kValue;
  auto &arr_a_data = this->arr_a_data;
  auto &arr_b_data = this->arr_b_data;
  auto &vec_a_data = this->vec_a_data;
  auto &vec_b_data = this->vec_b_data;

  auto &arr_a = this->arr_a;
  auto &arr_b = this->arr_b;
  auto &vec_a = this->vec_a;
  auto &vec_b = this->vec_b;
  // array by operator overload
  StdArray<kSize> res1 = array_operator_overload::calculate(arr_a_data, arr_b_data);
  {
    // vector by operator overload
    StdVector res2 = array_operator_overload::calculate(vec_a_data, vec_b_data);
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
  {
    // array&array by expression
    StdArray<kSize> res2 = calculate_by_expression(arr_a_data, arr_b_data);
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
  {
    // vector&vector by expression
    StdVector res2 = calculate_by_expression(vec_a_data, vec_b_data);
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
// et: expression template
#define SAME_TYPE(a, b, res)                                                                     \
  std::is_same_v<                                                                                \
      array_expression_template::ExprType<std::decay_t<decltype(a)>, std::decay_t<decltype(b)>>, \
      std::decay_t<decltype(res)>>
  {
    // array&array by et
    auto res2 = array_expression_template::calculate(arr_a, arr_b);
    static_assert(SAME_TYPE(arr_a, arr_b, res2));
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
  {
    // vector&vector by et
    auto res2 = array_expression_template::calculate(vec_a, vec_b);
    static_assert(SAME_TYPE(vec_a, vec_b, res2));
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
  {
    // array&vector by et
    auto res2 = array_expression_template::calculate(arr_a, vec_b);
    static_assert(SAME_TYPE(arr_a, vec_b, res2));
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
  {
    // vector&array by et
    auto res2 = array_expression_template::calculate(vec_a, arr_b);
    static_assert(SAME_TYPE(vec_a, arr_b, res2));
    for (auto i = 0; i < res2.size(); ++i) {
      ASSERT_NEAR(res1[i], res2[i], EPSILON);
    }
  }
}

REGISTER_TYPED_TEST_SUITE_P(ArrayCalculationTest, CheckEqual);
using SizeTypes =
    ::testing::Types<ArraySize<32>, ArraySize<256>, ArraySize<2048>, ArraySize<16384>>;
INSTANTIATE_TYPED_TEST_SUITE_P(DifferentSize, ArrayCalculationTest, SizeTypes);

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
