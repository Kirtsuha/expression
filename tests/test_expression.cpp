//
// Created by Кирилл on 15.03.2025.
//
#include <iostream>
#include "../src/expression.h"
#include <complex>
#include "../src/expression.cpp"
#include <cassert>

void test_value() {
    Expression<double> a(123.0);
    assert(a.eval({}) == 123.0);
    std::cout << "test_value: OK" << std::endl;
}

void test_variable() {
    Expression<double> a("lambda");
    assert(a.eval({{"lambda", 123}}) ==  123);
    std::cout << "test_variable: OK" << std::endl;
}

void test_binary_operators() {
    Expression<int> a("lambda");
    Expression<int> b(2);
    Expression<int> res = ((a*b) ^ a) + (b/b) - a;
    assert(res.eval({{"lambda",1}}) == 2);
    std::cout << "test_binary_operators: OK" << std::endl;
}

void test_unary_operators() {
    Expression<double> a(0);
    Expression<double> b(1);
    Expression<double> res = cos(a) + sin(a) + exp(a) + ln(b);
    assert(res.eval({}) == 2.0);
    std::cout << "test_unary_operators: OK" << std::endl;
}

void test_to_string() {
    Expression<int> a("a");
    Expression<int> b(1);
    Expression<int> res = (a*b) ^ a;
    assert(res.to_string() == "((a*1)^a)");
    std::cout << "test_to_string: OK" << std::endl;
}

void test_diff() {
    Expression<int> x("x");
    auto y = ln(x).diff("x");
    assert(y.to_string() == "(1/x)");
    std::cout << "test_diff: OK" << std::endl;
}

void test_substitute() {
    Expression<int> x("x");
    auto y = ln(x).diff("x");
    auto check = Expression<int>(1) / Expression<int>(123);
    assert(y.substitute({{"x", 123}}).to_string() == check.to_string());
    std::cout << "test_substitute: OK" << std::endl;
}

void test_complex() {
    using Expr = Expression<std::complex<double>>;
    Expr left("x");
    Expr right(std::complex<double>(0,1));
    auto check = left + right;
    assert(check.eval({{"x", std::complex<double>(1,0)}}) == std::complex<double>(1,1));
    std::cout << "test_complex: OK" << std::endl;
}

int main() {
    test_value();
    test_variable();
    test_binary_operators();
    test_unary_operators();
    test_to_string();
    test_diff();
    test_substitute();
    test_complex();
    

    return 0;
}
