#pragma once

#include "expr.hpp"
#include <iosfwd>

namespace exprs {
    /*
     * NUM
     */
    class num : public expr_base {
    public:
        explicit num(double &number);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        double number;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * VAR
     */
    class var : public expr_base {
    public:
        explicit var(std::string &variable);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        std::string variable;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * ADD
     */
    class add : public expr_base {
    public:
        add(expr &left, expr &right);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;

    private:
        expr left, right;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * SUB
     */
    class sub : public expr_base {
    public:
        sub(expr &left, expr &right);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;

    private:
        expr left, right;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * MUL
     */
    class mul : public expr_base {
    public:
        mul(expr &left, expr &right);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        expr left, right;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * DIV
     */
    class div : public expr_base {
    public:
        div(expr &left, expr &right);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        expr left, right;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * POW
     */
    class power : public expr_base {
    public:
        power(expr &left, expr &right);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        expr left, right;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * LOG
     */
    class logarithm : public expr_base {
    public:
        explicit logarithm(expr &left);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        expr left;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * SIN
     */
    class sinus : public expr_base {
    public:
        explicit sinus(expr &left);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        expr left;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };

    /*
     * COS
     */
    class cosinus : public expr_base {
    public:
        explicit cosinus(expr &left);
        double evaluate(const variable_map_t& variables) const override;
        expr derive(std::string const& variable) const override;
        expr simplify() const override;
    private:
        expr left;
        void write(std::ostream& out, WriteFormat fmt) const override;
        bool equals(const expr_base& b) const override;
    };
}
