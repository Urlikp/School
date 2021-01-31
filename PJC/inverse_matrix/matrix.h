#ifndef INVERSE_MATRIX_MATRIX_H
#define INVERSE_MATRIX_MATRIX_H

#include <iostream>
#include <iomanip>

class matrix {
    int dimension;
    long double **data;

public:
    explicit matrix(const int dim) : dimension(dim) {
        data = new long double*[dim];
        for (int i = 0; i < dim; i++) {
            data[i] = new long double[2 * dim];

            for (int j = dim; j < 2 * dim; j++) {
                data[i][j] = 0;
            }

            data[i][i + dim] = 1;
        }
    }

    ~matrix() {
        for (int i = 0; i < dimension; i++) {
            delete [] data[i];
        }
        delete [] data;
    }

    int get_dimension() const {
        return dimension;
    }

    void swap_rows(int first_row, int second_row);
    void multiply_and_sum_rows(int first_row, int second_row);
    void divide_row(int row);
    void set_value(int row, int col, double value);
    void print_matrix(bool only_right_side);
    bool is_zero_on_diagonal(int row);
};

#endif //INVERSE_MATRIX_MATRIX_H
