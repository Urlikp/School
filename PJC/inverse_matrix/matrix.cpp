#include "matrix.h"

//sum of first_row multiplied by first_coefficient and second_row multiplied by second_coefficient is saved to second_row
void matrix::multiply_and_sum_rows(int first_row, int second_row) {
    if (data[second_row][first_row] == 0.0f) {
        return;
    }

    double first_coefficient = -data[second_row][first_row], second_coefficient = data[first_row][first_row];

    for (int i = 0; i < 2 * dimension; i++) {
        data[second_row][i] = first_coefficient * data[first_row][i] + second_coefficient * data[second_row][i];
    }
}

void matrix::divide_row(int row) {
    for(int i = dimension; i < 2 * dimension; i++)
    {
        data[row][i] = data[row][i] / data[row][row];
    }
    data[row][row]=1;
}

void matrix::set_value(int row, int col, double value) {
    data[row][col] = value;
}

double matrix::get_value(int row, int col) {
    return data[row][col];
}

void matrix::print_matrix(bool only_right_side) {
    int col = 0;

    if (only_right_side) {
        col = dimension;
    }

    std::cout << std::setprecision(2) << std::fixed;

    for (int i = 0; i < dimension; i++) {
        for (int j = col; j < 2 * dimension; j++) {
            std::cout << ((std::abs(data[i][j]) < 0.005) ? 0: data[i][j]) << "\t";
        }

        std::cout << std::endl;
    }
}

void matrix::swap_rows(int first_row, int second_row) {
    double *temp;
    temp = data[first_row];
    data[first_row] = data[second_row];
    data[second_row] = temp;
}

bool matrix::is_zero_on_diagonal(int row) {
    return data[row][row] == 0;
}
