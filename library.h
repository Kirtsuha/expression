#ifndef EXPRSSION_LIBRARY_H
#define EXPRSSION_LIBRARY_H

#include <iostream>
#include <memory>
#include <string>
#include <map>

template <typename T>
class Node {
public:
    Node() = default;
    virtual ~Node() = default;
    virtual std::string to_string() = 0;
    virtual T eval(std::map<std::string,T> context) = 0;
};

template <typename T>
class Value : public Node<T> {
private:
    T value;
public:
    explicit Value(T val);
    ~Value() override;
    std::string to_string() override;
    T eval(std::map<std::string,T> context) override;
};

template <typename T>
class Variable : public Node<T> {
private:
    std::string name;
public:
    explicit Variable(std::string);
    ~Variable() override;
    std::string to_string() override;
    T eval(std::map<std::string,T> context) override;
};

template <typename T>
class BinaryOperation : public Node<T> {
private:
    std::shared_ptr<Node<T>> left;
    std::shared_ptr<Node<T>> right;
    char op;
public:
    explicit BinaryOperation(std::shared_ptr<Node<T>> l, std::shared_ptr<Node<T>> r, char o);
    ~BinaryOperation() override;
    std::string to_string() override;
    T eval(std::map<std::string,T> context) override;
};

template <typename T>
class UnaryOperation : public Node<T> {
private:
    std::shared_ptr<Node<T>> value;
    std::string op;
public:
    explicit UnaryOperation(std::shared_ptr<Node<T>> val, std::string o);
    ~UnaryOperation() override;
    std::string to_string() override;
    T eval(std::map<std::string,T> context) override;
};

template <typename T>
class Expression {
private:
    explicit Expression(std::shared_ptr<Node<T>> impl);
    std::shared_ptr<Node<T>> impl_;
public:
    explicit Expression(std::string variable);

    friend Expression operator""_val(T value);
    friend Expression operator""_var(const char* variable);
    //friend Expression operator""_var(const char* variable, size_t size);

    Expression operator+ (const Expression& that);
    Expression operator- (const Expression& that);
    Expression operator* (const Expression& that);
    Expression operator/ (const Expression& that);
    Expression operator^ (const Expression& that);

    Expression operator+=(const Expression& that);
    Expression operator-=(const Expression& that);
    Expression operator*=(const Expression& that);
    Expression operator/=(const Expression& that);
    Expression operator^=(const Expression& that);
};

#endif //EXPRSSION_LIBRARY_H
