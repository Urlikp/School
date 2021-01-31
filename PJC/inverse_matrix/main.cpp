#include "solver.h"
#include <chrono>
#include <algorithm>

#define DEFAULT_FILENAME "test.txt"

template <typename TimePoint>
std::chrono::milliseconds to_ms(TimePoint tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

// check if argument equals to help option
bool is_help(std::string const& argument) {
    return argument == "--help" || argument == "-h";
}

// check if argument equals to file option
bool is_file(std::string const& argument) {
    return argument == "--file" || argument == "-f";
}

// check if argument equals to thread option
bool multi_thread(std::string const& argument) {
    return argument == "--thread" || argument == "-t";
}

// print help
void print_help(std::string const& exe_name) {
    std::clog << "Usage: " << exe_name << " [OPTIONS]\n\n" << "Program to compute inverse matrix from input matrix saved in file (default name: test.txt).\n\n"
    << "Options:\n" << "-h\t\tPrint help and exit program\n" <<
    "-f <filename>\tSpecify that input matrix is in file <filename>\n";
}

int main(int argc, char* argv[]) {
    // if there is help option, print help and end the program
    if (std::any_of(argv, argv+argc, is_help)) {
        print_help(argv[0]);
        return 0;
    }

    solver *sol;

    // set values according to options
    if (argc == 4 && is_file(argv[1]) && multi_thread(argv[3])) {
        sol = new solver(argv[2]);
        sol->num_threads = std::thread::hardware_concurrency();
    } else if (argc == 4 && multi_thread(argv[1]) && is_file(argv[2])) {
        sol = new solver(argv[3]);
        sol->num_threads = std::thread::hardware_concurrency();
    } else if (argc == 3 && is_file(argv[1])) {
        sol = new solver(argv[2]);
    } else if (argc == 2 && multi_thread(argv[1])) {
        sol = new solver(DEFAULT_FILENAME);
        sol->num_threads = std::thread::hardware_concurrency();
    } else if (argc == 1) {
        sol = new solver(DEFAULT_FILENAME);
    } else {
        std::cerr << "Error: Unknown option\n";
        return 1;
    }

    // print input matrix
    std::cout << "Input matrix, including right side of the equations:\n";
    sol->print_matrix(false);
    std::cout << std::endl;

    // begin clock
    auto start = std::chrono::high_resolution_clock::now();

    // compute inverse matrix, if matrix is not regular, print message and end the program
    if (!sol->solve()) {
        std::cout << "Input matrix is not regular, therefore it's inverse matrix doesn't exist\n";
        delete sol;
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "Needed " << to_ms(end - start).count() << " ms to finish.\n";
        return 4;
    }

    // end clock
    auto end = std::chrono::high_resolution_clock::now();

    // print inverse matrix
    std::cout << "Inverse matrix computed from input matrix:\n";
    sol->print_matrix(true);
    std::cout << std::endl;

    delete sol;
    std::cout << "Needed " << to_ms(end - start).count() << " ms to finish.\n";
    return 0;
}
