#include "matrix.h"

// sum of first_row multiplied by first_coefficient and second_row multiplied by second_coefficient is saved to
// second_row
void matrix::multiply_and_sum_rows(int first_row, int second_row) {
    if (data[second_row][first_row] == 0.0f) {
        return;
    }

    long double first_coefficient = -data[second_row][first_row], second_coefficient = data[first_row][first_row];

    for (int i = 0; i < 2 * dimension; i++) {
        data[second_row][i] = first_coefficient * data[first_row][i] + second_coefficient * data[second_row][i];
    }
}

// divide every number, in the right side of the specified row, by number, from left side of the row, that is on the
// main diagonal
void matrix::divide_row(int row) {
    for(int i = dimension; i < 2 * dimension; i++)
    {
        data[row][i] = data[row][i] / data[row][row];
    }
    data[row][row]=1;
}

// sets value in matrix
void matrix::set_value(int row, int col, double value) {
    data[row][col] = value;
}

// print matrix, either only right side or the whole matrix
void matrix::print_matrix(bool only_right_side) {
    int col = 0;

    if (only_right_side) {
        col = dimension;
    }

    std::cout << std::setprecision(3) << std::fixed;

    for (int i = 0; i < dimension; i++) {
        for (int j = col; j < 2 * dimension; j++) {
            std::cout << ((std::abs(data[i][j]) < 0.0005) ? 0: data[i][j]) << "\t\t";
        }

        std::cout << std::endl;
    }
}

// swap two rows
void matrix::swap_rows(int first_row, int second_row) {
    long double *temp;
    temp = data[first_row];
    data[first_row] = data[second_row];
    data[second_row] = temp;
}

// check if there is a zero on position [row][row]
bool matrix::is_zero_on_diagonal(int row) {
    return data[row][row] == 0;
}
