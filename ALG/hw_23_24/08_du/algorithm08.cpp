//
// Created by vojte on 01.01.2024.
//

#include "algorithm08.h"

void compare_pos(int comp1, int comp2, int *to_set1, int *to_set2, int setter1, int setter2, int setter3, int setter4) {
    if (comp1 > comp2) {
        *to_set1 = comp1 + setter1;
        *to_set2 = setter2 + setter4;
    } else if (comp1 < comp2) {
        *to_set1 = comp2 + setter1;
        *to_set2 = setter3 + setter4;
    } else {
        if (setter2 < setter3) {
            *to_set1 = comp1 + setter1;
            *to_set2 = setter2 + setter4;
        } else {
            *to_set1 = comp2 + setter1;
            *to_set2 = setter3 + setter4;
        }
    }
}

void go_through(input_t *input, algorithm_t *algorithm) {
    int maximum = 0;
    int min_step = INT_MAX;
    bool last_row = false;

    // Iterate through rows from top to bottom
    for (int row = 0; row < input->m; row++) {
        if (row == input->m - 1) {
            last_row = true;
        }

        // Itarate through columns from left to right and calculate maximum
        for (int col = 0; col < input->n; col++) {
            int index = row * input->n + col;
            if (algorithm->calc_garden[index].value != -1) {
                // Compare left and top values
                int left = 0;
                int top = 0;

                int left_step = 0;
                int top_step = 0;

                if (col > 0 && algorithm->calc_garden[index - 1].value != -1) {
                    left = algorithm->calc_garden[index - 1].l_value;
                    left_step = algorithm->calc_garden[index - 1].l_step;
                }
                if (row > 0 && algorithm->calc_garden[index - input->n].value != -1) {
                    top = algorithm->calc_garden[index - input->n].value;
                    top_step = algorithm->calc_garden[index - input->n].step;
                }

                compare_pos(left,
                            top,
                            &algorithm->calc_garden[index].l_value,
                            &algorithm->calc_garden[index].l_step,
                            algorithm->calc_garden[index].value,
                            left_step,
                            top_step,
                            1);
            }
        }
        // Iterate through columns in left direction and calculate maximum
        for (int col = input->n - 1; col >= 0; col--) {
            int index = row * input->n + col;
            if (algorithm->calc_garden[index].value != -1) {
                // Compare right and top values
                int right = 0;
                int top = 0;

                int right_step = 0;
                int top_step = 0;

                if (col < input->n - 1 && algorithm->calc_garden[index + 1].value != -1) {
                    right = algorithm->calc_garden[index + 1].r_value;
                    right_step = algorithm->calc_garden[index + 1].r_step;
                }
                if (row > 0 && algorithm->calc_garden[index - input->n].value != -1) {
                    top = algorithm->calc_garden[index - input->n].value;
                    top_step = algorithm->calc_garden[index - input->n].step;
                }

                compare_pos(right,
                            top,
                            &algorithm->calc_garden[index].r_value,
                            &algorithm->calc_garden[index].r_step,
                            algorithm->calc_garden[index].value,
                            right_step,
                            top_step,
                            1);

                compare_pos(algorithm->calc_garden[index].l_value,
                            algorithm->calc_garden[index].r_value,
                            &algorithm->calc_garden[index].value,
                            &algorithm->calc_garden[index].step,
                            0,
                            algorithm->calc_garden[index].l_step,
                            algorithm->calc_garden[index].r_step,
                            0);


                if (last_row) {
                    if (algorithm->calc_garden[index].value > maximum) {
                        maximum = algorithm->calc_garden[index].value;
                        min_step = algorithm->calc_garden[index].step;
                    } else if (algorithm->calc_garden[index].value == maximum) {
                        int step = algorithm->calc_garden[index].step;
                        if (step < min_step) {
                            min_step = step;
                        }
                    }
                }
            }
        }
    }
    printf("%d %d\n", maximum, min_step);
}