//
// Created by vojte on 01.01.2024.
//

#include "utils.h"

// Define possible neighbors
int neighbors[4][2] = {{-1, 0},
                       {0,  1},
                       {1,  0},
                       {0,  -1}};

bool in_map(input_t *input, int row, int col) {
    return row >= 0 && row < input->m && col >= 0 && col < input->n;
}

// If flower, then all neighbours are += flower
void calc_neighbors(input_t *input, algorithm_t *algorithm, int row, int col, int index) {
    int value = input->garden[index];
    if (value > 0) {
        algorithm->calc_garden[index].value = -1;
        for (auto &neighbor: neighbors) {
            int n_row = row + neighbor[0];
            int n_col = col + neighbor[1];
            if (in_map(input, n_row, n_col)) {
                int neighbour_index = n_row * input->n + n_col;
                if (algorithm->calc_garden[neighbour_index].value != -1)
                    algorithm->calc_garden[neighbour_index].value += input->garden[index];
            }
        }
    }
}

// Read input from stdin
void read_input(input_t *input, algorithm_t *algorithm) {
    scanf("%d %d", &input->m, &input->n);

    input->garden = (int *) calloc(input->m * input->n, sizeof(int));
    algorithm->calc_garden = (position_t *) calloc(input->m * input->n, sizeof(position_t));

    input->empty = true;

    for (int row = 0; row < input->m; ++row) {
        for (int col = 0; col < input->n; ++col) {
            int index = row * input->n + col;
            int value;
            scanf("%d", &value);
            input->garden[index] = value;
            if(value > 0)
                input->empty = false;
            calc_neighbors(input, algorithm, row, col, index);
        }
    }
}

// Print input to stdout
void print_input(input_t *input, algorithm_t *algorithm) {
    printf("%d %d\n", input->m, input->n);

    for (int i = 0; i < input->m; ++i) {
        for (int j = 0; j < input->n; ++j) {
            int index = i * input->n + j;
            printf("G%d:C%d\t", input->garden[index], algorithm->calc_garden[index].value);
        }
        printf("\n");
    }
}
