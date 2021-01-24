#include "single_thread_solver.h"

bool single_thread_solver::solve() {
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
