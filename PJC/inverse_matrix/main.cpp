#include "solver.h"
#include <chrono>
#include <thread>
#include <algorithm>

#define DEFAULT_FILENAME "test.txt"

template <typename TimePoint>
std::chrono::milliseconds to_ms(TimePoint tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

bool is_help(std::string const& argument) {
    return argument == "--help" || argument == "-h";
}

bool is_file(std::string const& argument) {
    return argument == "--file" || argument == "-f";
}

void print_help(std::string const& exe_name) {
    std::clog << "Usage: " << exe_name << " [OPTIONS]\n\n" << "Program to compute inverse matrix from input matrix saved in file (default name: test.txt).\n\n"
    << "Options:\n" << "-h\t\tPrint help and exit program\n" <<
    "-f <filename>\tSpecify that input matrix is in file <filename>\n";
}

int main(int argc, char* argv[]) {
    auto start = std::chrono::high_resolution_clock::now();
    //unsigned int num_threads = std::thread::hardware_concurrency();

    if (std::any_of(argv, argv+argc, is_help)) {
        print_help(argv[0]);
        return 0;
    }

    solver *sol;
    if (argc == 3 && is_file(argv[1])) {
        sol = new solver(argv[2]);
    } else if (argc == 1) {
        sol = new solver(DEFAULT_FILENAME);
    } else {
        std::cerr << "Error: Unknown option\n";
        return 1;
    }

    std::cout << "Input matrix, including right side of the equations:\n";
    sol->print_matrix(false);
    std::cout << std::endl;

    if (!sol->solve()) {
        std::cout << "Input matrix is not regular, therefore it's inverse matrix doesn't exist\n";
        delete sol;
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Needed " << to_ms(end - start).count() << " ms to finish.\n";
        return 4;
    }

    std::cout << "Inverse matrix computed from input matrix:\n";
    sol->print_matrix(true);
    std::cout << std::endl;

    delete sol;
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Needed " << to_ms(end - start).count() << " ms to finish.\n";
    return 0;
}
