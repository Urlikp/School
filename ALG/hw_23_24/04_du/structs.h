//
// Created by vojte on 11.11.2023.
//

#ifndef INC_04_DU_STRUCTS_H
#define INC_04_DU_STRUCTS_H

#define NOT_FOUND (-1)
#define NOT_VALID (-2)
#define SAME 0
#define VISITED 1

struct position_t
{
    int row;
    int col;
};

struct input_t
{
    int m, n, c;
    int *map;
    int **color_map;
    bool **color_visited;

    position_t start, end;
};

struct cell_t
{
    position_t pos;
    int distance;

    int color_key;
};

#endif // INC_04_DU_STRUCTS_H
