#include "solver.h"

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
        exit(1);
    }
}

double solver::load_number(std::ifstream& input_file) {
    std::string str;
    input_file >> str;

    try {
        return std::stod(str);
    } catch (std::exception& exception) {
        std::cerr << exception.what();
        std::cerr << "File is not in required format!";
        exit(1);
    }
}

void solver::print_matrix(bool only_right_side) {
    mat->print_matrix(only_right_side);
}

bool solver::solve() {
    const int dimension = mat->get_dimension();

    for (int i = 0; i < dimension - 1; i++) {
        //kontrola jestli na hlavni diagonale jsou nenulova cisla a pripadne prohozeni radku
        for (int j = i + 1; j < dimension && mat->is_zero_on_diagonal(i); j++) {
            mat->swap_rows(i, j);
        }

        //pokud prohozeni radku nepomuze => konec programu
        if (mat->is_zero_on_diagonal(i)) {
            std::cout << "Regularni matici bych prosil..." << std::endl;
            return false;
        }

        //prevod do schodoviteho tvaru
        for (int j = i + 1; j < dimension; j++) {
            mat->multiply_and_sum_rows(i, j);
        }
    }

    //pokud po prevedeni do schodoviteho tvaru je clen v poslednim radku a v poslednim sloupci leve strany rovnic roven nule, pak matice neni regularni => konec programu
    if (mat->is_zero_on_diagonal(dimension - 1)) {
        std::cout << "Regularni matici bych prosil..." << std::endl;
        return false;
    }

    //prevod ze schodoviteho tvaru na diagonalni matici
    for (int i = dimension - 1; i >= 1; i--) {
        for (int j = i - 1; j>=0; j--) {
            mat->multiply_and_sum_rows(i, j);
        }
    }

    //vydeleni radku tak, aby jsme dostali na leve strane jednotkovou matici
    for (int i = 0; i < dimension; i++) {
        mat->divide_row(i);
    }

    return true;
}