//
// Created by vojte on 18.12.2023.
//

#include "algorithm.h"

void kdo_arr(input_t *input, alg_t *alg) {
    // Adding all subsets (small_k) elements to the set, except duplicates
    for (size_t j = 0; j < input->s; ++j) {
        std::vector<size_t> tmp(input->dict[j].begin() + 1, input->dict[j].begin() + input->k);
        // Check if tmp is in combinations
        alg->combinations.insert(tmp);
    }
    alg->kdo_size = alg->combinations.size();

    // Going through all the elements in the set and looking for the ones that are in dict
    for (auto kombinace: alg->combinations) {
        std::vector<size_t> kdo_arr_row(alg->combinations.size());
        std::vector<size_t> tmp(kombinace.begin(), kombinace.end()); // Newly created word to check
        for (size_t letter = 0; letter < input->num_of_letters; ++letter) {
            tmp.push_back(letter); // Add letter to the end of the word

            // Looking for all occurrences of tmp in dict
            for (size_t i = 0; i < input->s; ++i) {
                if (tmp == input->dict[i]) {
                    // Go through combinations and find right place
                    size_t indian = 0;
                    std::vector<size_t> smol_tmp(tmp.begin() + 1, tmp.begin() + input->k);

                    for (auto it: alg->combinations) {
                        if (std::equal(tmp.begin() + 1, tmp.end(), it.begin())) {
                            // Tmp is in combinations
                            if (kdo_arr_row.size() == alg->combinations.size())
                                kdo_arr_row[indian] = 1;
                            else
                                kdo_arr_row.push_back(1);
                        } else {
                            // Tmp is not in combinations
                            if (kdo_arr_row.size() == alg->combinations.size()) {
                                indian++;
                                continue;
                            } else
                                kdo_arr_row.push_back(0);
                        }
                        indian++;
                    }
                    break;
                }
            }
            tmp.pop_back(); // Remove letter from the end of the word
        }
        alg->kdo_arr.push_back(kdo_arr_row);
    }
}


void get_result(input_t *input, alg_t *alg) {
    // First iteration is to build array out of input
    std::vector<std::vector<size_t>> curr;
    for (int i = 0; i < input->s; ++i) {
        std::vector<size_t> curr_row;
        std::vector<size_t> tmp;
        // Fill tmp with letters from dict word
        for (int j = 1; j < input->k; ++j) {
            // Go through all letters in word
            tmp.push_back(input->dict[i][j]);
        }
        // Now find matching combination in alg->combinations
        int index = 0;
        for (auto it: alg->combinations) {
            if (std::equal(tmp.begin(), tmp.end(), it.begin())) {
                // Found matching combination
//                curr_row.insert(curr_row.end(), it.begin(), it.end());
                for (int j: alg->kdo_arr[index]) {
                    curr_row.push_back(j);
                }
                break;
            }
            index++;
        }
        curr.push_back(curr_row);
    }
// Now I have immediate result in curr
    size_t current = input->k + 1;
    size_t sum = 0;
    while (current < input->m) {
        sum = 0;
        // I have to iterate through this curr and create new according to combinations
        std::vector<std::vector<size_t >> new_curr;
        for (size_t dict_index = 0; dict_index < input->s; ++dict_index) {
            std::vector<size_t> curr_row(alg->combinations.size());

            for (size_t kdo_arr_index = 0; kdo_arr_index < alg->kdo_arr.size(); ++kdo_arr_index) {
                if (curr[dict_index][kdo_arr_index] > 0) {
                    // I have to add current according to KDO_Arr to the end of curr_row
//                    for (size_t num = 0; num < curr[dict_index][kdo_arr_index]; ++num) {
                    for (size_t k = 0; k < alg->kdo_arr[kdo_arr_index].size(); ++k) {
                        curr_row[k] += alg->kdo_arr[kdo_arr_index][k] * curr[dict_index][kdo_arr_index];
                        if (alg->kdo_arr[kdo_arr_index][k] == 1)
                            sum += curr[dict_index][kdo_arr_index];
                    }
//                    }
                }
            }
            new_curr.push_back(curr_row);
        }

        // Insert new_curr to curr
        curr = new_curr;

        current++;
    }

    printf("%zu\n", sum);

}