/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/27 13:05:33
# Desc    :
########################################################################
*/
#pragma once

#include "expression_impl.h"
#include "expression_template_deps/array/operator.h"

namespace array_expression_template {

#define OPERATOR_OVERLOAD(op, Expr)                          \
  template <IsExpr LExpr, IsExpr RExpr>                      \
  auto operator op(const LExpr& l_exp, const RExpr& r_exp) { \
    return Expr<LExpr, RExpr>(l_exp, r_exp);                 \
  }                                                          \
  template <class T, IsExpr RExpr>                           \
  auto operator op(const T& lhs, const RExpr& rhs) {         \
    return Expr<Variable<T>, RExpr>(Variable(lhs), rhs);     \
  }                                                          \
  template <IsExpr LExpr, class T>                           \
  auto operator op(const LExpr& lhs, const T& rhs) {         \
    return Expr<LExpr, Variable<T>>(lhs, Variable(rhs));     \
  }

template <IsExpr From, ArrayLike To>
void collect_result(const From& from, To* to) {
  auto size = from.size();
  for (auto i = 0; i < size; ++i) {
    (*to)[i] = from[i];
  }
}

OPERATOR_OVERLOAD(+, Add)
OPERATOR_OVERLOAD(*, Multiply)
}  // namespace array_expression_template
