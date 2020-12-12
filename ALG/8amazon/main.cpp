#include <cstdio>
#include <algorithm>

#define LINUX

int **production_consumption, **reward;
int village_count, main_village, cost;

inline void scan_int(int *x) {
#ifdef LINUX
    char c = (char)getchar_unlocked();
#else
    char c = (char)getchar();
#endif
    *x = 0;
    bool negative = false;

    while ((c < 48 || c > 57) && c != 45) {
#ifdef LINUX
        c = (char)getchar_unlocked();
#else
        c = (char)getchar();
#endif
    }

    if (c == 45) {
        negative = true;
#ifdef LINUX
        c = (char)getchar_unlocked();
#else
        c = (char)getchar();
#endif
    }

    while (c > 47 && c < 58) {
        *x = (int)((((*x) << 1) + ((*x) << 3)) + c - 48);
#ifdef LINUX
        c = (char)getchar_unlocked();
#else
        c = (char)getchar();
#endif
    }

    if (negative) {
        *x = -(*x);
    }
}

void load_input() {
    scan_int(&village_count);

    production_consumption = static_cast<int **>(calloc(village_count, sizeof(int *)));
    reward = static_cast<int **>(calloc(village_count, sizeof(int *)));

    if (production_consumption == nullptr || reward == nullptr) {
        fprintf(stderr, "Error: Bad allocation!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < village_count; i++) {
        production_consumption[i] = static_cast<int *>(calloc(village_count, sizeof(int)));
        reward[i] = static_cast<int *>(calloc(village_count, sizeof(int)));

        if (production_consumption[i] == nullptr || reward[i] == nullptr) {
            fprintf(stderr, "Error: Bad allocation!\n");
            exit(EXIT_FAILURE);
        }
    }

    scan_int(&main_village);
    main_village--;
    scan_int(&cost);

    int village;

    for (int i = 0; i < village_count; i++) {
        scan_int(&village);
        production_consumption[i][i] = village;
    }
}

void compute_production_consumption() {
    for (int i = 0; i < village_count; i++) {
        for (int j = i + 1; j < village_count; j++) {
            production_consumption[i][j] = production_consumption[i][j - 1] + production_consumption[j][j];
        }
    }
}

void compute_reward() {
    for (int i = 1; i < village_count; i++) {
        for (int j = i; j < village_count; j++) {
            int optimal = std::numeric_limits<int>::min();
            std::pair<int, int> index_reward(j - i, j);
            for (int k = 0; k < i; k++) {
                std::pair<int, int> index_1(index_reward.first, j - k - 1);
                std::pair<int, int> index_2(j - k, index_reward.second);
                int S_1 = production_consumption[index_1.first][index_1.second];
                int S_2 = production_consumption[index_2.first][index_2.second];
                int S_min = std::min(S_1, S_2);
                int S_max = std::max(S_1, S_2);
                int current_reward = -cost;

                if (S_min < 0 && S_max > 0) {
                    int S = std::min(abs(S_1), abs(S_2));

                    if (S > i) {
                        current_reward += S - i;
                    }
                }

                current_reward += reward[index_1.first][index_1.second] + reward[index_2.first][index_2.second];

                optimal = std::max(current_reward, optimal);
            }
            reward[index_reward.first][index_reward.second] = optimal;
        }
    }
}

int find_optimal() {
    int optimal = 0;

    for (int i = 0; i <= main_village; i++) {
        for (int j = main_village; j < village_count; j++) {
            optimal = std::max(reward[i][j], optimal);
        }
    }

    return optimal;
}

int main() {
    load_input();
    compute_production_consumption();
    compute_reward();
    int optimal = find_optimal();
    printf("%d\n", optimal);

    for (int i = 0; i < village_count; i++) {
        free(production_consumption[i]);
        free(reward[i]);
    }
    free(production_consumption);
    free(reward);
    return EXIT_SUCCESS;
}
