//
// Created by vojte on 18.12.2023.
//

#include "utils.h"

void read_input(input_t *input) {
    scanf("%d %d %d", &input->s, &input->k, &input->m);

    input->small_k = input->k - 1;

    std::set<size_t> letters;

    for (size_t i = 0; i < input->s; ++i) {
        std::vector<size_t> row;
        for (size_t j = 0; j < input->k; j++) {
            char c;
            scanf(" %c", &c);
            switch (c) {
                case 'A':
                    row.push_back(A);
                    letters.insert(A);
                    break;
                case 'B':
                    row.push_back(B);
                    letters.insert(B);
                    break;
                case 'C':
                    row.push_back(C);
                    letters.insert(C);
                    break;
                case 'D':
                    row.push_back(D);
                    letters.insert(D);
                    break;
                default:
                    break;
            }
        }
        input->dict.push_back(row);
    }
    input->num_of_letters = letters.size();
}

void print_input(input_t *input) {
    printf("%d %d %d\n", input->s, input->k, input->m);

    for (size_t i = 0; i < input->s; ++i) {
        for (size_t j = 0; j < input->k; j++) {
            /* code */
            switch (input->dict[i][j]) {
                case A:
                    printf("A");
                    break;
                case B:
                    printf("B");
                    break;
                case C:
                    printf("C");
                    break;
                case D:
                    printf("D");
                    break;
                default:
                    break;
            }
        }
        printf("\n");
    }
}

void print_kdo(alg_t *alg) {
    size_t i = 0;
    for (auto it: alg->combinations) {
        for (auto it2: it) {
            printf("%d", it2);
        }
        printf(" : ");
        for (size_t j = 0; j < alg->kdo_arr[i].size(); ++j) {
            printf("%d ", alg->kdo_arr[i][j]);
        }
        printf("\n");
        i++;
    }

}