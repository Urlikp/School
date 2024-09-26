//
// Created by vojte on 03.01.2024.
//

#ifndef INC_01_DU_NEW_UTILS_H
#define INC_01_DU_NEW_UTILS_H

#include "includes.h"

input_t read_input();

void print_2d_array(int *m, int n);

void print_input(input_t in);

void print_suitable_locs(queue<pair<int, int>> suitable_locations);
#endif //INC_01_DU_NEW_UTILS_H
