//
// Created by vojte on 03.01.2024.
//

#ifndef INC_01_DU_NEW_STRUCTS_H
#define INC_01_DU_NEW_STRUCTS_H

#include "includes.h"

struct input_t {
    int n{}; // size of map
    int k{}; // size of park
    int l{}; // distance from edge to center of park
    int s{}; // minimum number of rocks in center
    int *map; // values on map
    int c{}; // Size of center
};

struct prefixes_t {
    int *rocks;
    int *trees;
};

#endif //INC_01_DU_NEW_STRUCTS_H
