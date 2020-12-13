#pragma once

#include <memory>

enum class generator_kind {
    random,
    mersenne,
    minstd
};


class numbers_generator {
protected:
    double max_double, min_double;
    int max_int, min_int;
    size_t double_count = 0, int_count = 0, true_count = 0, false_count = 0, bool_count = 0;

public:
    /**
     * Vygeneruje náhodné číslo v intervalu [0, 1)
     */
    virtual double random_double() = 0;
    /**
     * Vygeneruje náhodné celé číslo v intervalu [lower, upper]
     */
    virtual int    random_int(int lower, int upper) = 0;
    /**
     * Vygeneruje náhodnou pravdivostní hodnotu (true/false)
     */
    virtual bool   random_bool() = 0;

    virtual ~numbers_generator() = default;

    static std::unique_ptr<numbers_generator> create_by_kind(generator_kind kind, size_t seed);

    double min_generated_double() const {return min_double;}
    double max_generated_double() const {return max_double;}
    size_t doubles_generated() const {return double_count;}
    int min_generated_int() const {return min_int;}
    int max_generated_int() const {return max_int;}
    size_t ints_generated() const {return int_count;}
    size_t trues_generated() const {return true_count;}
    size_t falses_generated() const {return false_count;}
    size_t bools_generated() const {return bool_count;}
};
