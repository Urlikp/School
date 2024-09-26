#include <iostream>
#include "includes.h"


int main() {
//    std::cout << "Hello, World!" << std::endl;

    input_t input{};
    read_input(&input);
//    print_input(&input);

    alg_t alg{};
    kdo_arr(&input, &alg);
//    print_kdo(&alg);

    get_result(&input, &alg);

    return 0;
}
