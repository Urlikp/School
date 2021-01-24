#include "single_thread_solver.h"

int main() {
    solver sol("test.txt");
    sol.print_matrix(true);
    std::cout << std::endl;
    sol.print_matrix(false);
    std::cout << std::endl;
    std::cout << sol.solve() << std::endl;
    sol.print_matrix(true);
    std::cout << std::endl;
    sol.print_matrix(false);
    std::cout << std::endl;
    return 0;
}
