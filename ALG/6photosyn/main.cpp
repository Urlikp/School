#include <cstdio>
#include <algorithm>
#include <vector>

#define LINUX

int *temperatures, *min_temperatures;
int temperature_count;

inline void scan_int(int *x) {
#ifdef LINUX
    char c = (char)getchar_unlocked();
#else
    char c = (char)getchar();
#endif
    *x = 0;
    while (c < 48 || c > 57) {
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
}

void load_input() {
    scan_int(&temperature_count);
    temperatures = static_cast<int *>(calloc(temperature_count, sizeof(int)));
    min_temperatures = static_cast<int *>(calloc(temperature_count, sizeof(int)));

    if (temperatures == nullptr || min_temperatures == nullptr) {
        fprintf(stderr, "Error: Bad allocation!\n");
        exit(EXIT_FAILURE);
    }

    int temperature;

    for (int i = 0; i < temperature_count; i++) {
        scan_int(&temperature);
        temperatures[i] = temperature;
    }
}

int find_optimal() {
    int min_temperature = temperatures[0];
    for (size_t i = 0; i < temperature_count; i++) {
        min_temperature = std::min(min_temperature, temperatures[i]);
        min_temperatures[i] = min_temperature;
    }

    int max_temperature = temperatures[temperature_count - 1];
    int optimal = 0, quality;
    for (int i = temperature_count - 1; i >= 0; i--) {
        max_temperature = std::max(max_temperature, temperatures[i]);
        quality = std::min(temperatures[i] - min_temperatures[i], max_temperature - temperatures[i]);
        optimal = std::max(quality, optimal);
    }
    return optimal;
}

int main() {
    load_input();
    int optimal = find_optimal();
    printf("%d\n", optimal);
    free(temperatures);
    free(min_temperatures);
    return EXIT_SUCCESS;
}
