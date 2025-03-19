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
    RIGHT_BRACKET,
    EQUAL
};

struct Token {
    TokenType ttype;
    std::string value;
};

std::vector<Token> tokenizer(std::string input, bool is_eqaul_expected) {
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
        if (input[i] == '=') {
            if (is_eqaul_expected) {
                result.push_back({EQUAL, "="});
                continue;
            }
            throw std::runtime_error("Equal sign is not accepted");
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
    auto tokenized = tokenizer(input, false);
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
            std::vector<Token> tokens = tokenizer(expr_str, false);
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
                    case EQUAL:
                        std::cout << "EQUAL";
                        break;
                }
                std::cout << ", Value: " << token.value << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            return 1;
        }
        std::vector<Token> tokens = tokenizer(expr_str, false);
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


    } else if (mode == "--eval") {
        std::cout << "Input expression: " << expr_str << std::endl;
        bool is_complex = 0;
        std::vector<std::vector<Token>> tokened_args;

        for (int i = 3; i < argc; i++) {
            std::vector<Token> temp = tokenizer(argv[i], true);

            if (temp.size() != 5 && temp.size() != 7) {
                throw std::runtime_error("Wrong args usage: [name]=[number | real_part+imag_part]");
            }
            tokened_args.push_back(temp);
            for (size_t j = 0; j < temp.size(); j++) {
                if (temp[j].ttype == COMPLEX_NUMBER) {
                    is_complex = true;
                }
            }
        }

        std::vector<Token> tokens = tokenizer(expr_str, false);
        std::vector<Token> polish = polish_order(tokens);

        if (is_complex) {
            std::map<std::string,std::complex<double>> context;
            for (size_t i = 0; i < tokened_args.size(); i++) {
                auto temp = tokened_args[i];
                
                for (size_t j = 0; j < temp.size(); j++) {
                    if (temp.size() == 5 && 
                    temp[0].ttype == LEFT_BRACKET &&
                    temp[1].ttype == VARIABLE && 
                    temp[2].ttype == EQUAL && 
                    temp[3].ttype == NUMBER &&
                    temp[4].ttype == RIGHT_BRACKET) {
                        context[temp[1].value] = std::complex<double>(std::stod(temp[3].value), 0.0);
                    } 
                    else if (temp.size() == 5 && 
                    temp[0].ttype == LEFT_BRACKET &&
                    temp[1].ttype == VARIABLE && 
                    temp[2].ttype == EQUAL && 
                    temp[3].ttype == COMPLEX_NUMBER &&
                    temp[4].ttype == RIGHT_BRACKET) {
                        context[temp[1].value] = std::complex<double>(0.0, std::stod(temp[3].value));
                    } 
                    else if (temp.size() == 7 && 
                    temp[0].ttype == LEFT_BRACKET &&
                    temp[1].ttype == VARIABLE && 
                    temp[2].ttype == EQUAL &&
                    temp[3].ttype == NUMBER && 
                    temp[4].value == "+" &&
                    temp[5].ttype == COMPLEX_NUMBER &&
                    temp[6].ttype == RIGHT_BRACKET) {
                        context[temp[1].value] = std::complex<double>(std::stod(temp[3].value), std::stod(temp[5].value));
                    }
                    else if (temp.size() == 7 && 
                    temp[0].ttype == LEFT_BRACKET &&
                    temp[1].ttype == VARIABLE && 
                    temp[2].ttype == EQUAL &&
                    temp[3].ttype == NUMBER && 
                    temp[4].value == "-" &&
                    temp[5].ttype == COMPLEX_NUMBER &&
                    temp[6].ttype == RIGHT_BRACKET) {
                        context[temp[1].value] = std::complex<double>(std::stod(temp[3].value), -std::stod(temp[5].value));
                    }
                    else {
                        throw std::runtime_error("Wrong for " + temp[1].value +"Args usage: [name]=[number | real_part(+-)imag_part]");
                    }
                }
            }
            auto ans = parser<std::complex<double>>(expr_str);
            std::cout << ans.eval(context) << std::endl;
        } else {
            std::map<std::string,double> context;
            for (size_t i = 0; i < tokened_args.size(); i++) {
                auto temp = tokened_args[i];
                if (temp.size() == 5 &&
                temp[0].ttype == LEFT_BRACKET &&
                temp[1].ttype == VARIABLE &&
                temp[2].ttype == EQUAL && 
                temp[3].ttype == NUMBER &&
                temp[4].ttype == RIGHT_BRACKET) {
                    context[temp[1].value] = std::stod(temp[3].value);
                } else {
                    throw std::runtime_error("Args usage: [name]=[number | real_part+imag_part]");
                }
            }
            auto ans = parser<double>(expr_str);
            std::cout << ans.eval(context)<< std::endl;
        }
        
    }
    
    else if (mode == "--diff") {
        std::cout << "Input expression: " << expr_str << std::endl;
        std::vector<Token> tokens = tokenizer(expr_str, false);
        std::vector<Token> polish = polish_order(tokens);
        bool is_complex = false;
        if (argc != 4) {
            throw std::runtime_error("Args usage: [name]");
        }
        std::string name = argv[3];
        for (size_t i = 0; i < polish.size(); i++) {
            if (polish[i].ttype == COMPLEX_NUMBER) {
                is_complex = true;
            }
        }
        if (is_complex) {
            auto ans = parser<std::complex<double>>(expr_str);
            std::cout << ans.diff(name).to_string();
        } else {
            auto ans = parser<double>(expr_str);
            std::cout << ans.diff(name).to_string();
        }
    }
    else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return 1;
    }

    return 0;
}