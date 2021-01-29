#ifndef INVERSE_MATRIX_SOLVER_H
#define INVERSE_MATRIX_SOLVER_H

#include <string>
#include <fstream>
#include "matrix.h"

class solver {
    double load_number(std::ifstream& input_file);
protected:
    matrix *mat;
public:
    void load_input(const std::string& file_name);
    void print_matrix(bool only_right_side);
    bool solve();

    explicit solver(const std::string& file_name) {
        load_input(file_name);
    }

    ~solver() {
        delete mat;
    }
};


#endif //INVERSE_MATRIX_SOLVER_H
