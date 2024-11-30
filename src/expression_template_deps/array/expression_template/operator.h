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

namespace array_expression_template {

#define OPERATOR_OVERLOAD(op, Expr) \
template <IsExpr LExpr, IsExpr RExpr> \
Expr<LExpr, RExpr> operator op (const LExpr& l_exp, const RExpr& r_exp) { \
    return Expr(l_exp, r_exp); \
} \
template <class T, IsExpr RExpr> \
Expr<Variable<T>, RExpr> operator op (const T& lhs, const RExpr& rhs) { \
    return Expr(Variable(lhs), rhs); \
} \
template <IsExpr LExpr, class T> \
Expr<LExpr, Variable<T>> operator op (const LExpr& lhs, const T& rhs) { \
    return Expr(lhs, Variable(rhs)); \
}

OPERATOR_OVERLOAD(+, Add)
OPERATOR_OVERLOAD(*, Multiply)

}  // namespace array_expression_template
