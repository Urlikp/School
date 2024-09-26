#include "includes.h"


// * MAIN ----------------------------------------------------------------------

int main() {

    input_t *input = new input_t;

    algorithm_t *alg = new algorithm_t;
    alg->t_max = INT_MAX;
    alg->balanc_max = 0;

    read_input(input, alg);
    // print_input();

    algorithm(input, alg);

    free(input->boxes);
    free(input->tree_array);
    free(input);

    printf("%d %d", alg->balanc_max, alg->t_max);
    return 0;
}