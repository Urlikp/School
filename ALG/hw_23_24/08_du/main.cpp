#include "includes.h"

int main() {
    input_t input{};
    algorithm_t algorithm{};

    read_input(&input, &algorithm);
//    print_input(&input, &algorithm);

    int zero = 0;

    if (!input.empty)
        go_through(&input, &algorithm);
    else
        printf("%d %d\n", zero, input.m);

    return 0;
}
