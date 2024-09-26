//
// Created by vojte on 03.01.2024.
//

#ifndef INC_03_DU_STRUCTS_H
#define INC_03_DU_STRUCTS_H

#include "includes.h"

struct node_t
{
    int cities;
    int depth;

    int id;
    node_t *left;
    node_t *right;
};

struct input_t
{
    int n;

    node_t *root;
    int *values;
    node_t **tree_array;
};


#endif //INC_03_DU_STRUCTS_H
