//
// Created by vojte on 01.01.2024.
//

#ifndef INC_08_DU_UTILS_H
#define INC_08_DU_UTILS_H

#include "includes.h"

void compare_pos(int comp1, int comp2, int *to_set1, int *to_set2, int setter1, int setter2, int setter3, int setter4) ;
void calc_neighbors(input_t *input, algorithm_t *algorithm, int row, int col, int index);
void read_input(input_t *input, algorithm_t *algorithm);
void print_input(input_t *input, algorithm_t *algorithm);

#endif //INC_08_DU_UTILS_H
