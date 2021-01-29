#include "solver.h"

// load input from <file_name>
void solver::load_input(const std::string& file_name) {
    std::ifstream file;
    file.open(file_name);

    if (file.is_open()) {
        int dim = (int)load_number(file);

        mat = new matrix(dim);

        double value;

        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                value = load_number(file);
                mat->set_value(i, j, value);
            }
        }

        file.close();
    } else {
        std::cerr << "File doesn't exist.";
        exit(2);
    }
}

// load individual numbers
double solver::load_number(std::ifstream& input_file) {
    std::string str;
    input_file >> str;

    try {
        return std::stod(str);
    } catch (std::exception& exception) {
        std::cerr << exception.what() << std::endl;
        std::cerr << "File is not in required format!";
        delete mat;
        exit(3);
    }
}

void solver::print_matrix(bool only_right_side) {
    mat->print_matrix(only_right_side);
}

// compute the inverse matrix using Gauss–Jordan elimination
bool solver::solve() {
    const int dimension = mat->get_dimension();

    for (int i = 0; i < dimension - 1; i++) {
        // check if there is zero on main diagonal, if yes swap rows
        for (int j = i + 1; j < dimension && mat->is_zero_on_diagonal(i); j++) {
            mat->swap_rows(i, j);
        }

        // if swapping rows doesn't help => end of program
        if (mat->is_zero_on_diagonal(i)) {
            return false;
        }

        // transfer to Row echelon form
        for (int j = i + 1; j < dimension; j++) {
            mat->multiply_and_sum_rows(i, j);
        }
    }

    // if, after transfer to Row echelon form, the number at the last cell in the last column is equal to 0, than matrix
    // is not regular => end of program
    if (mat->is_zero_on_diagonal(dimension - 1)) {
        return false;
    }

    // transfer from Row echelon form to diagonal matrix
    for (int i = dimension - 1; i >= 1; i--) {
        for (int j = i - 1; j>=0; j--) {
            mat->multiply_and_sum_rows(i, j);
        }
    }

    // divide the rows so we get identity matrix on left side
    for (int i = 0; i < dimension; i++) {
        mat->divide_row(i);
    }

    return true;
}