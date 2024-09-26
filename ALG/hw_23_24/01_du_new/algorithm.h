//
// Created by vojte on 03.01.2024.
//

#ifndef INC_01_DU_NEW_ALGORITHM_H
#define INC_01_DU_NEW_ALGORITHM_H

#include "includes.h"

void calc_prefix_sum(input_t in, prefixes_t *pref);

int calc_sum_in_pref(const int *prefix_sum, int n, int up_row, int up_col, int down_row, int down_col);

int suitable_locs(input_t in, prefixes_t pref);

#endif // INC_01_DU_NEW_ALGORITHM_H
