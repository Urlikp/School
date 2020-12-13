#include "expr.hpp"
#include "expr_impl.hpp"
#include <stdexcept>
#include "tokenizer.hpp"
#include <sstream>
#include <stack>

const expr expr::ZERO = expr::number(0.0);
const expr expr::ONE = expr::number(1.0);

// TODO: overloaded operators +, -, *, /, functions power, logarithm, sinus, cosinus,
//       expr::number, expr::variable, operator==, operator<<,
//       create_expression_tree

void add_expression(Token &top, std::stack<expr> &result) {
    if(result.empty()) {
        throw parse_error("! parse_error");
    }

    if (top.id == TokenId::Plus) {
        expr b = result.top();
        result.pop();
        if(result.empty()) {
            throw parse_error("! parse_error");
        }
        expr a = result.top();
        result.pop();
        result.push(a + b);
    } else if (top.id == TokenId::Minus) {
        expr b = result.top();
        result.pop();
        if(result.empty()) {
            throw parse_error("! parse_error");
        }
        expr a = result.top();
        result.pop();
        result.push(a - b);
    } else if (top.id == TokenId::Multiply) {
        expr b = result.top();
        result.pop();
        if(result.empty()) {
            throw parse_error("! parse_error");
        }
        expr a = result.top();
        result.pop();
        result.push(a * b);
    } else if (top.id == TokenId::Divide) {
        expr b = result.top();
        result.pop();
        if(result.empty()) {
            throw parse_error("! parse_error");
        }
        expr a = result.top();
        result.pop();
        result.push(a / b);
    } else if (top.id == TokenId::Power) {
        expr b = result.top();
        result.pop();
        if(result.empty()) {
            throw parse_error("! parse_error");
        }
        expr a = result.top();
        result.pop();
        result.push(pow(a, b));
    } else if (top.identifier == "sin") {
        expr a= result.top();
        result.pop();
        result.push(sin(a));
    } else if (top.identifier == "cos") {
        expr a = result.top();
        result.pop();
        result.push(cos(a));
    } else if (top.identifier == "log") {
        expr a = result.top();
        result.pop();
        result.push(log(a));
    }
}

expr create_expression_tree(const std::string& expression) {
    std::istringstream istream(expression);
    Tokenizer tokenizer(istream);
    Token token = tokenizer.next();
    std::stack<expr> result;
    std::stack<Token> operators;

    while(token.id != TokenId::End) {
        if(token.id == TokenId::Number) {
            result.push(expr::number(token.number));
        } else if (token.id == TokenId::Identifier) {
            if(token.identifier == "sin" || token.identifier == "cos" || token.identifier == "log") {
                operators.push(token);
            } else {
                result.push(expr::variable(token.identifier));
            }
        } else if (token.is_binary_op()) {
            while (!operators.empty()) {
                Token top = operators.top();

                if (top.id == TokenId::LParen  || top.id == TokenId::RParen) {
                    break;
                }

                if (top.identifier == "sin" || top.identifier == "cos" || top.identifier == "log") {
                    add_expression(top, result);
                    operators.pop();
                } else if ((token.associativity() == Associativity::Left && token.op_precedence() <= top.op_precedence())
                        || (token.associativity() == Associativity::Right && token.op_precedence() < top.op_precedence())) {
                    add_expression(top, result);
                    operators.pop();
                }
                else {
                    break;
                }
            }
            operators.push(token);
        } else if (token.id == TokenId::LParen) {
            operators.push(token);
        } else if (token.id == TokenId::RParen) {
            while(!operators.empty()) {
                Token top = operators.top();
                operators.pop();
                if(top.id == TokenId::LParen) {
                    break;
                }
                if(operators.empty()) {
                    throw parse_error("! parse_error");
                }
                add_expression(top, result);
            }
        }

        token = tokenizer.next();
    }

    if(!operators.empty()) {
        if(operators.top().id == TokenId::LParen || operators.top().id == TokenId::RParen) {
            throw parse_error("! parse_error");
        }
        while(!operators.empty()) {
            add_expression(operators.top(), result);
            operators.pop();
        }
    }

    if(result.empty()) {
        throw parse_error("! parse_error");
    }

    return result.top();
}

bool operator==(const expr &a, const expr &b) {
    a->equals(dynamic_cast<expr_base>(b));
    // TODO
    throw std::logic_error("not implemented");
}

std::ostream& operator<<(std::ostream &os, const expr &e) {
    e->write(os, expr::WriteFormat::Infix);
    return os;
}

expr expr::number(double n) {
    return std::make_shared<exprs::num>(n);
}

expr expr::variable(std::string name) {
    return std::make_shared<exprs::var>(name);
}

expr operator+(expr a, expr b) {
    return std::make_shared<exprs::add>(a,b);
}

expr operator-(expr a, expr b) {
    return std::make_shared<exprs::sub>(a, b);
}

expr operator*(expr a, expr b) {
    return std::make_shared<exprs::mul>(a, b);
}

expr operator/(expr a, expr b) {
    return std::make_shared<exprs::div>(a, b);
}

expr pow(expr m, expr e) {
    return std::make_shared<exprs::power>(m, e);
}

expr log(expr e) {
    return std::make_shared<exprs::logarithm>(e);
}

expr sin(expr e) {
    return std::make_shared<exprs::sinus>(e);
}

expr cos(expr e) {
    return std::make_shared<exprs::cosinus>(e);
}
