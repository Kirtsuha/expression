#include "expression.h"

#include <iostream>
#include <string>
#include <cmath>
#include <string>
#include <memory>
#include <complex>

template <typename T>
Value<T>::Value(T val) : value(val) {}

template<typename T>
Value<T>::~Value() = default;

template<typename T>
std::string Value<T>::to_string() {
    if constexpr (std::is_same_v<T, std::complex<double>>) {
        return "(" + std::to_string(value.real()) + "," + std::to_string(value.imag()) + ")";
    } else {
        return std::to_string(value);
    }
}

template<typename T>
T Value<T>::eval(std::map<std::string, T> context) {
    return value;
}

template<typename T>
std::shared_ptr<Node<T>> Value<T>::diff(std::string name) {
    return std::make_shared<Value<T>>(0.0);
}

template<typename T>
std::shared_ptr<Node<T>> Value<T>::substitute(std::map<std::string, T> context) {
    return std::make_shared<Value<T>>(value);
}

template<typename T>
Variable<T>::Variable(std::string n) : name(n) {}

template<typename T>
Variable<T>::~Variable() = default;

template<typename T>
std::string Variable<T>::to_string() {
    return name;
}

template<typename T>
T Variable<T>::eval(std::map<std::string, T> context) {
    auto it = context.find(name);
    if (it != context.end()) {
        return context[name];
    }
    throw std::runtime_error("Variable not found: " + name);
    return 0;
}

template<typename T>
std::shared_ptr<Node<T>> Variable<T>::diff(std::string var) {
    if (var == name) {
        return std::make_shared<Value<T>>(1.0);
    } return std::make_shared<Value<T>>(0.0);
}

template<typename T>
std::shared_ptr<Node<T>> Variable<T>::substitute(std::map<std::string, T> context) {
    auto it = context.find(name);
    if (it != context.end()) {
        return std::make_shared<Value<T>>(context[name]);
    }
    return std::make_shared<Variable<T>>(name);
}

template<typename T>
BinaryOperation<T>::BinaryOperation(std::shared_ptr<Node<T>> l, std::shared_ptr<Node<T>> r, char o)
    : left(l), right(r), op(o) {}

template<typename T>
BinaryOperation<T>::~BinaryOperation() = default;

template<typename T>
std::string BinaryOperation<T>::to_string() {
    return "(" + left->to_string() + op + right->to_string() + ")";
}

template<typename T>
T BinaryOperation<T>::eval(std::map<std::string, T> context) {
    T a = left->eval(context);
    T b = right->eval(context);
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return a / b;
        case '^': return pow(a, b);
        default : throw std::runtime_error("Unknown operation: " + op);
    }
}

template<typename T>
std::shared_ptr<Node<T>> BinaryOperation<T>::diff(std::string name) {
    switch (op) {
        case '+': return std::make_shared<BinaryOperation<T>>(left->diff(name), right->diff(name), '+');
        case '-': return std::make_shared<BinaryOperation<T>>(left->diff(name), right->diff(name), '-');
        case '*' : {
            auto new_left = BinaryOperation<T>(left->diff(name), right, '*');
            auto new_right = BinaryOperation<T>(left, right->diff(name), '*');
            return std::make_shared<BinaryOperation<T>>(std::make_shared<BinaryOperation<T>>(new_left), std::make_shared<BinaryOperation<T>>(new_right), '+');
        }
        case '/' : {
            auto new_left  = std::make_shared<BinaryOperation<T>>(left->diff(name), right, '*');
            auto new_right = std::make_shared<BinaryOperation<T>>(left, right->diff(name), '*');
            auto numerator = std::make_shared<BinaryOperation<T>>(new_left, new_right, '-');
            auto denominator = std::make_shared<BinaryOperation<T>>(right, std::make_shared<Value<T>>(2), '^');
            return std::make_shared<BinaryOperation<T>>(numerator, denominator, '/');
        }
        case '^' : {
            auto left_part1 = std::make_shared<BinaryOperation<T>>(
                left, std::make_shared<BinaryOperation<T>>(right, std::make_shared<Value<T>>(1),'-'), '^');
            auto left_part2 = std::make_shared<BinaryOperation<T>>(
                right, left_part1, '*');
            auto left_part3 = std::make_shared<BinaryOperation<T>>(
                left_part2, left->diff(name), '*');

            auto right_part1 = std::make_shared<BinaryOperation<T>>(
                left, right, '^');
            auto right_part2 = std::make_shared<BinaryOperation<T>>(
                right_part1, std::make_shared<UnaryOperation<T>>(left, "ln"), '*');
            auto right_part3 = std::make_shared<BinaryOperation<T>>(
                right_part2, right->diff(name), '*');

            return std::make_shared<BinaryOperation<T>>(
                left_part3, right_part3, '+');
        }
        default: throw std::runtime_error("Unknown operation: " + op);
    }
}

template<typename T>
std::shared_ptr<Node<T>> BinaryOperation<T>::substitute(std::map<std::string, T> context) {
    return std::make_shared<BinaryOperation<T>>(left->substitute(context), right->substitute(context), op);
}

template<typename T>
UnaryOperation<T>::UnaryOperation(std::shared_ptr<Node<T>> val, std::string o)
    : value(val), op(o) {}

template<typename T>
UnaryOperation<T>::~UnaryOperation() = default;

template<typename T>
std::string UnaryOperation<T>::to_string() {
    return op + "(" + value->to_string() + ")";
}

template<typename T>
T UnaryOperation<T>::eval(std::map<std::string, T> context) {
    T a = value->eval(context);
    if (op == "sin") return std::sin(a);
    if (op == "cos") return std::cos(a);
    if (op == "ln") return std::log(a);
    if (op == "exp") return std::exp(a);
    throw std::runtime_error("Unknown function: " + op);
}

template<typename T>
std::shared_ptr<Node<T>> UnaryOperation<T>::diff(std::string name) {
    if (op == "sin") {
        return std::make_shared<BinaryOperation<T>>(
            std::make_shared<UnaryOperation<T>>(value, "cos"), value->diff(name), '*');
    }
    if (op == "cos") {
        return std::make_shared<BinaryOperation<T>>(
            std::make_shared<BinaryOperation<T>>(std::make_shared<UnaryOperation<T>>(value, "sin"),std::make_shared<Value<T>>(-1),'*'),
            value->diff(name), '*');
    }
    if (op == "ln") {
        return std::make_shared<BinaryOperation<T>>(
            value->diff(name), value, '/');
    }
    if (op == "exp") {
        return std::make_shared<BinaryOperation<T>>(
            std::make_shared<UnaryOperation<T>>(value, "exp"), value->diff(name), '*');
    }
    throw std::runtime_error("Unknown function: " + op);
}

template<typename T>
std::shared_ptr<Node<T>> UnaryOperation<T>::substitute(std::map<std::string, T> context) {
    return std::make_shared<UnaryOperation<T>>(value->substitute(context),op);
}

template<typename T>
Expression<T>::Expression(std::shared_ptr<Node<T>> impl) : impl_(impl) {}

template<typename T>
Expression<T>::Expression(std::string variable) : impl_(std::make_shared<Variable<T>>(variable)) {}

template<typename T>
Expression<T>::Expression(T value) : impl_(std::make_shared<Value<T>>(value)) {}


template<typename T>
T Expression<T>::eval(std::map<std::string, T> context) {
    return impl_->eval(context);
}

template<typename T>
Expression<T> Expression<T>::diff(std::string name) {
    return Expression<T>(impl_->diff(name));
}

template<typename T>
Expression<T> Expression<T>::substitute(std::map<std::string, T> context) {
    return Expression<T>(impl_->substitute(context));
}

template<typename T>
std::string Expression<T>::to_string() {
    return impl_->to_string();
}

template<typename T>
std::shared_ptr<Node<T>> Expression<T>::node() {
    return impl_;
}

template<typename T>
Expression<T> operator+(const Expression<T>& lhs, const Expression<T>& rhs) {
    return Expression<T>(std::make_shared<BinaryOperation<T>>(lhs.impl_, rhs.impl_, '+'));
}

template<typename T>
Expression<T> operator-(const Expression<T>& lhs, const Expression<T>& rhs) {
    return Expression<T>(std::make_shared<BinaryOperation<T>>(lhs.impl_, rhs.impl_, '-'));
}

template<typename T>
Expression<T> operator*(const Expression<T>& lhs, const Expression<T>& rhs) {
    return Expression<T>(std::make_shared<BinaryOperation<T>>(lhs.impl_, rhs.impl_, '*'));
}

template<typename T>
Expression<T> operator/(const Expression<T>& lhs, const Expression<T>& rhs) {
    return Expression<T>(std::make_shared<BinaryOperation<T>>(lhs.impl_, rhs.impl_, '/'));
}

template<typename T>
Expression<T> operator^(const Expression<T>& lhs, const Expression<T>& rhs) {
    return Expression<T>(std::make_shared<BinaryOperation<T>>(lhs.impl_, rhs.impl_, '^'));
}

template<typename T>
Expression<T> sin(const Expression<T>& that) {
    return Expression<T>(std::make_shared<UnaryOperation<T>>(that.impl_, "sin"));
}

template<typename T>
Expression<T> cos(const Expression<T>& that) {
    return Expression<T>(std::make_shared<UnaryOperation<T>>(that.impl_, "cos"));
}

template<typename T>
Expression<T> exp(const Expression<T>& that) {
    return Expression<T>(std::make_shared<UnaryOperation<T>>(that.impl_, "exp"));
}

template<typename T>
Expression<T> ln(const Expression<T>& that) {
    return Expression<T>(std::make_shared<UnaryOperation<T>>(that.impl_, "ln"));
}

/*
int main() {
    using Expr = Expression<std::complex<double>>;

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
*/
