//
// Created by vojte on 03.01.2024.
//

#ifndef INC_02_DU_ALGORITHM_H
#define INC_02_DU_ALGORITHM_H

#include "includes.h"
int time_calc(input_t *input, room_t *rooms);
int balance_calc(input_t *input, room_t *rooms);
bool is_valid_pos(node_t *node, room_t *rooms);
void go_through(input_t *input, algorithm_t *alg, room_t *rooms, int placed_boxes, int curr_box);
void algorithm(input_t *input, algorithm_t *alg);

#endif //INC_02_DU_ALGORITHM_H
