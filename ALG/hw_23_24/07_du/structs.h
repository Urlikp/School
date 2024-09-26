//
// Created by vojte on 18.12.2023.
//

#ifndef INC_07_DU_STRUCTS_H
#define INC_07_DU_STRUCTS_H

#include "includes.h"

struct input_t {
    size_t s, k, m;
    size_t small_k;
    std::vector<std::vector<size_t>> dict;
    std::size_t num_of_letters;
};

struct alg_t {
    std::set<std::vector<size_t>> combinations;

    std::vector<std::vector<size_t>> kdo_arr;
    std::size_t kdo_size;

};

#endif //INC_07_DU_STRUCTS_H
