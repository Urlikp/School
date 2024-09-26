//
// Created by vojte on 01.01.2024.
//

#ifndef INC_08_DU_STRUCTS_H
#define INC_08_DU_STRUCTS_H

#include "includes.h"

struct input_t {
    int m;
    int n;
    bool empty;
    int *garden;
};

struct position_t {
    int l_value, r_value;
    int value;
    int l_step, r_step;
    int step;
};

struct algorithm_t {
    position_t *calc_garden;
};

#endif //INC_08_DU_STRUCTS_H
