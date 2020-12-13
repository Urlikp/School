#include "expr_impl.hpp"

#include <iostream>
#include <cmath>
#include <limits>
#include <typeinfo>

namespace exprs {
    /*
     * NUM
     */
    num::num(double &number) {
        this->number = number;
    }

    double num::evaluate(const variable_map_t& variables) const {
        return this->number;
    }

    expr num::derive(std::string const& variable) const {
        return expr::ZERO;
    }

    expr num::simplify() const {
        return expr::number(this->number);
    }

    void num::write(std::ostream& out, WriteFormat fmt) const {
        out << this->number;
    }

    bool num::equals(const expr_base& b) const {
        
        num &d = dynamic_cast<num&>(b);
        //exprs::num* test = dynamic_cast<exprs::num*>(b);
        //expr* test = dynamic_cast<expr*>(b);
        if (test != 0) {
            return this->number ==test->number;
        }
        return false;
    }

    /*
     * VAR
     */
    var::var(std::string &variable) {
        this->variable = variable;
    }

    double var::evaluate(const variable_map_t& variables) const {
        auto itr = variables.begin();
        itr = variables.find(this->variable);

        if (itr == variables.end()) {
            throw unbound_variable_exception("! unbound_variable_exception");
        }

        return itr->second;
    }

    expr var::derive(std::string const& variable) const {
        if (this->variable == variable) {
            return expr::ONE;
        }
        return expr::ZERO;
    }

    expr var::simplify() const {
        return expr::variable(this->variable);
    }

    void var::write(std::ostream& out, WriteFormat fmt) const {
        out << this->variable;
    }

    bool var::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * ADD
     */
    add::add(expr &left, expr &right) {
        this->left = left;
        this->right = right;
    }

    double add::evaluate(const variable_map_t& variables) const {
        return this->left->evaluate(variables) + this->right->evaluate(variables);
    }

    expr add::derive(std::string const& variable) const {
        return this->left->derive(variable) + this->right->derive(variable);
    }

    expr add::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void add::write(std::ostream& out, WriteFormat fmt) const {
        out << "(+ " << this->left << " " << this->right << ")";
    }

    bool add::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * SUB
     */
    sub::sub(expr &left, expr &right) {
        this->left = left;
        this->right = right;
    }

    double sub::evaluate(const variable_map_t& variables) const {
        return this->left->evaluate(variables) - this->right->evaluate(variables);
    }

    expr sub::derive(std::string const& variable) const {
        return this->left->derive(variable) - this->right->derive(variable);
    }

    expr sub::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void sub::write(std::ostream& out, WriteFormat fmt) const {
        out << "(- " << this->left << " " << this->right << ")";
    }

    bool sub::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * MUL
     */
    mul::mul(expr &left, expr &right) {
        this->left = left;
        this->right = right;
    }

    double mul::evaluate(const variable_map_t& variables) const {
        return this->left->evaluate(variables) * this->right->evaluate(variables);
    }

    expr mul::derive(std::string const& variable) const {
        return this->left->derive(variable) * this->right + this->left * this->right->derive(variable);
    }

    expr mul::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void mul::write(std::ostream& out, WriteFormat fmt) const {
        out << "(* " << this->left << " " << this->right << ")";
    }

    bool mul::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * DIV
     */
    div::div(expr &left, expr &right) {
        this->left = left;
        this->right = right;
    }

    double div::evaluate(const variable_map_t& variables) const {
        return this->left->evaluate(variables) / this->right->evaluate(variables);
    }

    expr div::derive(std::string const& variable) const {
        return (this->left->derive(variable) * this->right - this->left * this->right->derive(variable))/(pow(this->right, expr::number(2)));
    }

    expr div::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void div::write(std::ostream& out, WriteFormat fmt) const {
        out << "(/ " << this->left << " " << this->right << ")";
    }

    bool div::equals(const expr_base& b) const{
        throw std::logic_error("not implemented yet");
    }

    /*
     * POW
     */
    power::power(expr &left, expr &right) {
        this->left = left;
        this->right = right;
    }

    double power::evaluate(const variable_map_t& variables) const {
        return std::pow(this->left->evaluate(variables), this->right->evaluate(variables));
    }

    expr power::derive(std::string const& variable) const {
        return pow(this->left, this->right) * ((this->left->derive(variable) * this->right)/this->left + log(this->left) * this->right->derive(variable));
    }

    expr power::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void power::write(std::ostream& out, WriteFormat fmt) const {
        out << "(^ " << this->left << " " << this->right << ")";
    }

    bool power::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * LOG
     */
    logarithm::logarithm(expr &left) {
        this->left = left;
    }

    double logarithm::evaluate(const variable_map_t& variables) const {
        return std::log(this->left->evaluate(variables));
    }

    expr logarithm::derive(std::string const& variable) const {
        return this->left->derive(variable)/this->left;
    }

    expr logarithm::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void logarithm::write(std::ostream& out, WriteFormat fmt) const {
        out << "(log " << this->left << ")";
    }

    bool logarithm::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * SIN
     */
    sinus::sinus(expr &left) {
        this->left = left;
    }

    double sinus::evaluate(const variable_map_t& variables) const {
        return std::sin(this->left->evaluate(variables));
    }

    expr sinus::derive(std::string const& variable) const {
        return cos(this->left) * this->left->derive(variable);
    }

    expr sinus::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void sinus::write(std::ostream& out, WriteFormat fmt) const {
        out << "(sin " << this->left << ")";
    }

    bool sinus::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }

    /*
     * COS
     */
    cosinus::cosinus(expr &left) {
        this->left = left;
    }

    double cosinus::evaluate(const variable_map_t& variables) const {
        return std::cos(this->left->evaluate(variables));
    }

    expr cosinus::derive(std::string const& variable) const {
        return (expr::ZERO - sin(this->left)) * this->left->derive(variable);
    }

    expr cosinus::simplify() const {
        throw std::logic_error("not implemented yet");
    }

    void cosinus::write(std::ostream& out, WriteFormat fmt) const {
        out << "(cos " << this->left << ")";
    }

    bool cosinus::equals(const expr_base& b) const {
        throw std::logic_error("not implemented yet");
    }
} // namespace exprs
