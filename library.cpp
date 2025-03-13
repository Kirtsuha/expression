#include "library.h"

#include <iostream>
#include <string>
#include <cmath>

template <typename T>
Value<T>::Value(T val) : value(val) {}

template<typename T>
Value<T>::~Value() = default;

template<typename T>
std::string Value<T>::to_string() {
    return std::to_string(value);
}

template<typename T>
T Value<T>::eval(std::map<std::string, T> context) {
    return value;
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
    switch (op) {
        case "sin" : sin(a);
        case "cos" : cos(a);
        case "log" : log(a);
        case "exp" : exp(a);
        default : throw std::runtime_error("Unknown function: " + op);
    }
}

template<typename T>
Expression<T>::Expression(std::shared_ptr<Node<T>> impl) : impl_(impl) {}

template<typename T>
Expression<T>::Expression(std::string variable) : impl_(Node<T>(variable)) {}



int main() {
    auto left = Value<double>(123);
    auto right = Variable<double>("lambda");
    auto op = BinaryOperation<double>(std::shared_ptr<Node<double> >(&left), std::shared_ptr<Node<double> >(&right), '^');

    std::cout << op.to_string() << std::endl;
    std::map<std::string, double> context = {{"lambda", 2.0}};
    std::cout << op.eval(context);
    return 0;
}


