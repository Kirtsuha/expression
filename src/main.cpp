#include "expression.h"
#include "expression.cpp"
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

enum TokenType {
    NUMBER,
    COMPLEX_NUMBER,
    VARIABLE,
    OPERATION,
    FUNCTION,
    LEFT_BRACKET,
    RIGHT_BRACKET
};

struct Token {
    TokenType ttype;
    std::string value;
};

std::vector<Token> tokenizer(std::string input) {
    std::vector<Token> result;
    result.push_back({LEFT_BRACKET, "("});
    for (size_t i = 0; i < input.size(); i++) {
        if (input[i] == ' ') {
            continue;
        }
        if (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/' || input[i] == '^') {
            result.push_back({OPERATION,std::string(1,input[i])});
            continue;
        }
        if (isdigit(input[i]) || input[i] == '.') {
            std::string number = "";
            while (i < input.size() && (isdigit(input[i]) || input[i] == '.' || input[i] == 'i')) {
                number += input[i];
                i++;
            }
            i--;
            if (input[i] == 'i') {
                result.push_back({COMPLEX_NUMBER,number});
            } else {
                result.push_back({NUMBER,number});
            }
            continue;
        }
        if (input[i] == '(') {
            result.push_back({LEFT_BRACKET, "("});
            continue;
        }
        if (input[i] == ')') {
            result.push_back({RIGHT_BRACKET, ")"});
            continue;
        }
        if (isalpha(input[i])) {
            std::string lambda;
            while (i < input.size() && isalpha(input[i])) {
                lambda += input[i];
                ++i;
            }
            i--;
            if (lambda == "sin" || lambda == "cos" || lambda == "exp" || lambda == "ln") {
                result.push_back({FUNCTION, lambda});
            } else {
                result.push_back({VARIABLE, lambda});
            }
            continue;
        }
        throw std::runtime_error("Unknown symbole: " + input[i]);
    }
    result.push_back({RIGHT_BRACKET, ")"});
    return result;
}

std::vector<Token> polish_order(std::vector<Token> input) {
    std::map<std::string,int> priority = {
        {"(", 0},
        {"+", 1},
        {"-", 1},
        {"*", 2},
        {"/", 2},
        {"^", 3}, 
        {"sin", 4},
        {"cos", 4},
        {"exp", 4},
        {"ln", 4}
    };
    std::vector<Token> stack;
    std::vector<Token> result;
    for (size_t i = 0; i < input.size(); i++) {
        auto c = input[i];
        if (c.ttype == NUMBER || c.ttype == COMPLEX_NUMBER || c.ttype == VARIABLE) {
            result.push_back(c);
        }
        if (c.ttype == LEFT_BRACKET) {
            stack.push_back(c);
        }
        if (c.ttype == OPERATION || c.ttype == FUNCTION) {
            while (stack.size() > 0) {
                size_t temp = stack.size() - 1;
                if (priority[stack[temp].value] > priority[c.value]) {
                    result.push_back(stack[temp]);
                    stack.pop_back();
                } else {
                    break;
                }
            }
            stack.push_back(c);
        }
        if (c.ttype == RIGHT_BRACKET) {
            while (stack.size() > 0) {
                size_t temp = stack.size() - 1;
                if (stack[temp].ttype == LEFT_BRACKET) {
                    stack.pop_back();
                    break;
                }
                result.push_back(stack[temp]);
                stack.pop_back();
            }
        }
    }
    return result;
}

template <typename T>
Expression<T> rec_lexer_double(std::vector<Token>& input) {
    auto temp = input[input.size() - 1];
    input.pop_back();
    if constexpr(std::is_same_v<T, std::complex<double>>) {
        if (temp.ttype == NUMBER) {
            std::complex<double> x(std::stod(temp.value), 0.0);
            return Expression<T>(x);
        }
        if (temp.ttype == COMPLEX_NUMBER) {
            temp.value.pop_back();
            std::complex<double> x(0.0, std::stod(temp.value));
            return Expression<T>(x);
        }
    } else {
        if (temp.ttype == NUMBER) {
            return Expression<T>(std::stod(temp.value));
        }
    }

    if (temp.ttype == VARIABLE) {
        return Expression<T>(temp.value);
    }
    if (temp.ttype == OPERATION) {
        auto right = rec_lexer_double<T>(input);
        auto left = rec_lexer_double<T>(input);
        switch (temp.value[0]) {
            case '+': return left + right;
            case '-': return left - right;
            case '*': return left * right;
            case '/': return left / right;
            case '^': return left ^ right;
            default: throw std::runtime_error("Unknown operation: " + temp.ttype);
        }
    }
    if (temp.ttype == FUNCTION) {
        auto value = rec_lexer_double<T>(input);
        if (temp.value == "sin") return sin(value);
        if (temp.value == "cos") return cos(value);
        if (temp.value == "exp") return exp(value);
        if (temp.value == "ln") return ln(value);
        throw std::runtime_error("Unknown function: " + temp.value);
    }
    throw std::runtime_error("Unknown token type: " + std::to_string(temp.ttype));

}

template <typename T>
Expression<T> parser(std::string input) {
    auto tokenized = tokenizer(input);
    auto sorted = polish_order(tokenized);
    return rec_lexer_double<T>(sorted);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: differentiator [--eval|--diff] <expression> [args]\n";
        return 1;
    }

    std::string mode = argv[1];
    std::string expr_str = argv[2];

    if (mode == "--parse") {
        try {
            std::vector<Token> tokens = tokenizer(expr_str);
            for (const auto& token : tokens) {
                std::cout << "Token type: ";
                switch (token.ttype) {
                    case NUMBER:
                        std::cout << "NUMBER";
                        break;
                    case COMPLEX_NUMBER:
                        std::cout << "COMPLEX_NUMBER";
                        break;
                    case VARIABLE:
                        std::cout << "VARIABLE";
                        break;
                    case OPERATION:
                        std::cout << "OPERATION";
                        break;
                    case FUNCTION:
                        std::cout << "FUNCTION";
                        break;
                    case LEFT_BRACKET:
                        std::cout << "LEFT_BRACKET";
                        break;
                    case RIGHT_BRACKET:
                        std::cout << "RIGHT_BRACKET";
                        break;
                }
                std::cout << ", Value: " << token.value << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        std::vector<Token> tokens = tokenizer(expr_str);
        std::vector<Token> polish = polish_order(tokens);
        std::cout << "Result ordered in polish notation: ";
        for (size_t i = 0; i < polish.size(); i++) {
            std::cout << polish[i].value << " ";
        }
        std::cout << std::endl;

        bool is_complex = false;
        for (size_t i = 0; i < polish.size(); i++) {
            if (polish[i].ttype == COMPLEX_NUMBER) {
                is_complex = true;
            }
        }
        if (is_complex) {
            auto result = parser<std::complex<double>>(expr_str);
            std::cout << result.to_string() << std::endl;
        } else {
            auto result = parser<double>(expr_str);
            std::cout << result.to_string() << std::endl;
        }
    } /*else if (mode == "--diff") {
        std::string var_name = argv[4];
        auto expr = parse_expression(expr_str);
        auto derivative = expr.derivative(var_name);
        std::cout << derivative.toString() << "\n";
    } */
    else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    return 0;
}