//
// Created by Кирилл on 15.03.2025.
//
#include <iostream>
#include "library.h"
#include <complex>
#include "library.cpp"

int main() {
    using Expr = Expression<std::complex<double>>;
    /*auto left = Value<double>(123);
    auto right = Variable<double>("lambda");
    auto op = BinaryOperation<double>(std::shared_ptr<Node<double> >(&left), std::shared_ptr<Node<double> >(&right), '^');

    std::cout << op.to_string() << std::endl;
    std::map<std::string, double> context = {{"lambda", 3.0}};
    std::cout << op.eval(context) << std::endl;*/

    std::complex<double> num1(2.0, 3.0);
    std::complex<double> num2(1.0, -1.0);

    Expr left_expr(num1);
    Expr right_expr("phi");
    Expr op_expr = sin(left_expr + sin(right_expr) * left_expr) - (right_expr ^ left_expr) / right_expr;
    Expr new_expr = std::move(op_expr);
    std::cout << new_expr.to_string() << std::endl;
    std::cout << new_expr.eval({{"phi", num2}}) << std::endl;
    Expr plus = right_expr ^ left_expr;

    std::cout << plus.diff("phi").to_string() << std::endl;
    std::cout << plus.diff("phi").substitute({{"phi", num2}}).to_string() << std::endl;

    Expr three("x");
    Expr op = ln(three).diff("x");
    std::cout << op.to_string();

    return 0;
}
