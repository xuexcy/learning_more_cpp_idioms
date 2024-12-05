/**
########################################################################
#
# Copyright (c) 2024 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author  :   xuechengyun
# E-mail  :   xuechengyun@gmail.com
# Date    :   2024/11/23 14:06:56
# Desc    :
########################################################################

不要在代码运行时提前一步一步计算，通过程序在编译时使用模板将需要计算的表达式展开，在程序运行时一次计算(少创建临时变量、少读、少写)
在整个表达式之前(调用赋值操作符之前)不要尝试计算表达式。在求值之前，必须记录哪些操作应用于哪些对象。操作在编译时确定，
因此可以对模板参数进行编码。
@ref:
https://www.cnblogs.com/chengxuyuancc/p/3238469.html
https://zhuanlan.zhihu.com/p/701819779

好处:
1. 性能优化
    避免不必要的临时对象创建和赋值
    实现表达式的融合和优化，例如将多个连接的计算合并为一个更高效的计算过程
2. 类型安全
    确保在表达式计算过程中的类型一致性和正确性，减少类型错误的发生
3. 泛型变成
    方便实现通用的算法和数据结构，能够处理各种不同类型的表达式
*/

#include <print>
#include <vector>

#include "expression_template_deps/array/expression_template/expression.h"
#include "utils/main_decorator.h"

namespace bad_case {
void foo(int x) {
  std::println("value: {}", x);
}
void run_executed_before_function() {
  int x{20};
  // 函数调用，表达式提前计算
  foo(x + x + x);
}

int expression(int x) {
  return x + x + x;
}
void foo(int x, int (*expr)(int)) {
  std::println("value: {}", expr(x));
}

void run_with_expression() {
  int x{20};
  // 延时计算: 将表达式作为参数，在需要使用表达式结果时进行计算
  foo(x, expression);
}
}  // namespace bad_case

namespace BinaryExpression {
struct Var {
  double operator()(double v) { return v; }
};  // struct Var

struct Constant {
  double c;
  Constant(double d) : c(d) {}
  double operator()(double) { return c; }
};  // struct Constant

template <class L, class H, class OP>
struct DBinaryExpression {
  L l_;
  H h_;
  DBinaryExpression(L l, H h) : l_(l), h_(h) {}
  double operator()(double d) { return OP::apply(l_(d), h_(d)); }
};  // struct DBinaryExpression

struct Add {
  static double apply(double l, double h) { return l + h; }
};  // struct Add

template <class E>
struct DExpression {
  E expr_;
  DExpression(E e) : expr_(e) {}
  double operator()(double d) { return expr_(d); }
};  // struct DExpression

template <class Itr, class Func>
void evaluate(Itr begin, Itr end, Func func) {
  for (Itr i = begin; i != end; ++i) {
    std::println("{}", func(*i));
    // func(*i)
    // expr.operator()(*i)
    // vl_adder.operator()(*i)
    // Add::apply(l_(*i), h_(*i))
    // Add::apply(x(*i), l(*i))
    // Add::apply(Var().operator()(*i), Constant(50.00).operator()(*i))
    // Add::apply(*i, 50.00)
    // *i + 50.00
  }
}
}  // namespace BinaryExpression

namespace BinaryExpression {
void run() {
  using Variable = DExpression<Var>;
  using Literal = DExpression<Constant>;
  using VarLitAdder = DBinaryExpression<Variable, Literal, Add>;
  using MyAdder = DExpression<VarLitAdder>;

  // DExpression<Var> x(Var())
  Variable x((Var()));
  // DExpression<Constant> l(Constant(50.00))
  Literal l(Constant(50.00));
  // DBinaryExpression<DExpression<Var>, DExpression<Constant>, Add>
  VarLitAdder vl_adder(x, l);
  // DExpression<DBinaryExpression<DExpression<Var>, DExpression<Constant>,
  // Add>>
  MyAdder expr(vl_adder);

  std::vector<double> a{10, 20};
  evaluate(a.begin(), a.end(), expr);

  // TODO
  // evaluate(a.begin(), a.end(), )
}
}  // namespace BinaryExpression

void run_array() {
  std::array<double, 10> arr_a;
  std::array<double, 10> arr_b;
  using namespace array_expression_template;
  Array a(arr_a);
  Array b(arr_b);
  auto expr = a + a * a + 3.4 + 0.2 + b * a + 1.1 * 1.2 + 1.2 * a * 0.3 * 1.2 * b + b * a * a +
              a * b + 0.2 * b;
  std::println("{}", expr[0]);
}

int main() {
  utils::MainDecorator::Access();
  bad_case::run_executed_before_function();
  bad_case::run_with_expression();
  // test_array::run_array_calculation_compare();
  return 0;
}
