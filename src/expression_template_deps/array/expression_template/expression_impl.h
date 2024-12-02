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

struct Expression {};  // struct Expression

template <class T>
concept IsExpr = std::derived_from<T, Expression>;

template <class T>
class Variable : public Expression {
public:
    using item_type = std::decay_t<T>;
    explicit Variable(const T& t): t_(t) {}
    inline const item_type& operator[](size_t) const {
        return t_;
    }
    inline constexpr size_t size() const { return 0; }
private:
    const T t_;  // 这里没有 &
};  // class Variable

template <ArrayLike T>
class Array : public Expression {
public:
    using item_type = ItemType<T>;
    explicit Array(const T& t): t_(t) { assert(0 != size()); }
    inline const item_type& operator[](size_t idx) const {
        return t_[idx];
    }
    inline size_t size() const { return t_.size(); }
private:
    const T& t_;  // 这里需要 &
};  // class Array

template <IsExpr LExpr, IsExpr RExpr>
requires SameItem<LExpr, RExpr>
class Add: public Expression {
public:
    using item_type = LExpr::item_type;
    Add(const LExpr& l_expr, const RExpr& r_expr): l_expr_(l_expr), r_expr_(r_expr) {
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
    inline item_type operator[](size_t idx) const {
        return l_expr_[idx]+r_expr_[idx];
    }
    inline size_t size() const {
        return size_;
    }
private:
    const LExpr l_expr_;
    const RExpr r_expr_;
    size_t size_{0};
};  // class Add


template <IsExpr LExpr, IsExpr RExpr>
requires SameItem<LExpr, RExpr>
class Multiply : public Expression {
public:
    using item_type = LExpr::item_type;
    Multiply(const LExpr& l_expr, const RExpr& r_expr): l_expr_(l_expr), r_expr_(r_expr) {
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
    inline item_type operator[](size_t idx) const {
        return l_expr_[idx] * r_expr_[idx];
    }
    inline size_t size() const {
        return size_;
    }
private:
    const LExpr l_expr_;
    const RExpr r_expr_;
    size_t size_{0};
};  // class Multiply

}  // namespace array_expression_template


