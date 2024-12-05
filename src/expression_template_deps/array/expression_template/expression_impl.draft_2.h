/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/26 15:34:42
# Desc    :
########################################################################
*/
#pragma once

#include <cassert>
#include <cstddef>
#include <print>

#include "expression_template_deps/array/concepts.h"

namespace array_expression_template {

struct Expression {
};  // struct Expression

template <class T>
concept IsExpr = std::derived_from<T, Expression>;

#define DEFINE_IS_TYPE(class_name) \
template <class... Ts> \
struct Is##class_name: public std::false_type {}; \
template <class... Ts> \
struct Is##class_name<class_name<Ts...>> : public std::true_type {}; \

template <class T>
class Variable : public Expression {
public:
    using item_type = std::decay_t<T>;
    explicit Variable(const T& t): t_(t) {}
    __attribute__((always_inline)) inline const item_type& operator[](size_t) const {
        return t_;
    }
    inline constexpr size_t size() const { return 0; }
//protected:
    const T t_;  // 这里没有 &
};  // class Variable

template <ArrayLike T>
class Array : public Expression {
public:
    using item_type = ItemType<T>;
    explicit Array(const T& t): t_(t) { assert(0 != size()); }
    __attribute__((always_inline)) inline const item_type& operator[](size_t idx) const {
        return t_[idx];
    }
    inline size_t size() const { return t_.size(); }
//protected:
    const T& t_;  // 这里需要 &
};  // class Array

template <IsExpr LExpr, IsExpr RExpr>
requires SameItem<LExpr, RExpr>
class BinaryExpression : public Expression {
public:
    using item_type = LExpr::item_type;
    BinaryExpression(const LExpr& l_expr, const RExpr& r_expr): l_expr_(l_expr), r_expr_(r_expr) {
        // 0 无法区别 Variable 和 empty array
        // 不过我们在 Array 的构造函数里 assert(0 != size())
        // 因为表达式中含有 empty array 是 invalid/没有意义 的
        if (0 == l_expr_.size()) {
            size_ = r_expr_.size();
        } else if (0 == r_expr_.size()) {
            size_ = l_expr_.size();
        } else {
            assert(l_expr_.size() == r_expr_.size());
            size_ = l_expr_.size();
        }
    }
    inline size_t size() const {
        return size_;
    }
protected:
    const LExpr l_expr_;
    const RExpr r_expr_;
    size_t size_{0};
};  // class BinaryExpression

#define DEFINE_BINARY_EXPRESSION(class_name) \
template <class LExpr, class RExpr> \
class class_name : public BinaryExpression<LExpr, RExpr> { \
    using Base = BinaryExpression<LExpr, RExpr>; \
public: \
    using item_type = Base::item_type; \
    using Base::Base; \
    inline item_type operator[](size_t idx) const; \
}; \
DEFINE_IS_TYPE(class_name)

DEFINE_BINARY_EXPRESSION(Add)
DEFINE_BINARY_EXPRESSION(Multiply)


DEFINE_IS_TYPE(Variable);
DEFINE_IS_TYPE(Array);







#define EXPAND_VARIABLE(expr)  expr.t_
#define EXPAND_ARRAY(expr, i) expr.t_[i]
// multiply need (),  expr: (1 + 2) * (2 + 3)
// if no (), 1 + 2 * 2 + 3
#define EXPAND_BINARY_EXPR(expr, i, op)  (EXPAND_EXPR(expr.l_expr_, i)) op (EXPAND_EXPR(expr.r_expr_, i))

// 不支持这种语法
#define EXPAND_EXPR(expr, i)  \
    IsVariable<decltype(expr)>::value ? EXPAND_VARIABLE(expr) : \
    (IsArray<decltype(expr)>::value ? EXPAND_ARRAY(expr, i) : \
    (IsAdd<decltype(expr)>::value ? EXPAND_BINARY_EXPR(expr, i, +) : \
    (IsMultiply<decltype(expr)>::value ? EXPAND_BINARY_EXPR(expr, i, *) : \
       0 \
    )))

#define DEFINE_EXPRESSION_INDEX_OPERATOR(op, class_name) \
template <class LExpr, class RExpr> \
__attribute__((always_inline)) inline auto class_name<LExpr, RExpr>::operator[](size_t idx) const -> class_name<LExpr, RExpr>::item_type { \
    return Base::l_expr_[idx] op Base::r_expr_[idx]; \
}
    //return EXPAND_EXPR(expr, i);
    ///return EXPAND_EXPR((*this), idx); \

DEFINE_EXPRESSION_INDEX_OPERATOR(+, Add)
DEFINE_EXPRESSION_INDEX_OPERATOR(*, Multiply)

}  // namespace array_expression_template


