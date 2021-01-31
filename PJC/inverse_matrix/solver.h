#ifndef INVERSE_MATRIX_SOLVER_H
#define INVERSE_MATRIX_SOLVER_H

#include <string>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include "matrix.h"

class solver {
    double load_number(std::ifstream& input_file);
    matrix *mat;
    int remaining_rows = 0, current_row = 0;
    std::mutex remaining_rows_mutex;
public:
    unsigned int num_threads = 1;

    void load_input(const std::string& file_name);
    void print_matrix(bool only_right_side);
    void row_echelon_form(int dimension);
    void diagonal_matrix();
    bool solve();

    explicit solver(const std::string& file_name) {
        load_input(file_name);
    }

    ~solver() {
        delete mat;
    }
};


#endif //INVERSE_MATRIX_SOLVER_H
