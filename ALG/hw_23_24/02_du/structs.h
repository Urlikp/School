//
// Created by vojte on 03.01.2024.
//

#ifndef INC_02_DU_STRUCTS_H
#define INC_02_DU_STRUCTS_H

#include "includes.h"

using namespace std;

struct node_t
{
    int id;
    node_t *left = nullptr;
    node_t *right = nullptr;
    int parent_id;
    int time_to_get_here;
};

struct input_t
{
    int n;
    int b;
    int *boxes;
    node_t **tree_array;
};

struct algorithm_t
{
    int t_max = INT_MAX;
    int balanc_max;
};

struct room_t
{
    int sum;
    int num_of_boxes;
};

#endif // INC_02_DU_STRUCTS_H
