#include <cstdio>
#include <algorithm>
#include <vector>
#include <queue>
#include <map>

#define LINUX

struct city {
    const int id;
    int to_road_count = 0;
    bool zeroth_meridian_city = false;

    std::pair<int, int> cheapest_price_from = {-1, std::numeric_limits<int>::max()};

    std::vector<std::pair<int, int>> neighbours;
    std::vector<int> sort_neighbours;

    explicit city(const int id) : id(id) {
    }
};

int city_count, road_count, zeroth_meridian_city_count, circulation_count, optimal_price = std::numeric_limits<int>::max();

std::map<int, int> zeroth_meridian_cities_indexes;

std::vector<struct city *> cities;
std::vector<struct city *> topological_sorted_cities;
std::vector<int> zeroth_meridian_cities;
std::vector<std::vector<int>> cheapest_path;
std::vector<std::vector<int>> current_optimal;

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
    scan_int(&city_count);
    scan_int(&road_count);
    scan_int(&zeroth_meridian_city_count);
    scan_int(&circulation_count);

    for (int i = 0; i < city_count; i++) {
        cities.push_back(new struct city(i));
    }

    cheapest_path = std::vector<std::vector<int>>(zeroth_meridian_city_count, std::vector<int>(zeroth_meridian_city_count, -1));
    current_optimal = std::vector<std::vector<int>>(zeroth_meridian_city_count, std::vector<int>(circulation_count, std::numeric_limits<int>::max()));

    int city_id;

    for (int i = 0; i < zeroth_meridian_city_count; i++) {
        scan_int(&city_id);
        city_id--;
        zeroth_meridian_cities.push_back(city_id);
        cities[city_id]->zeroth_meridian_city = true;
        zeroth_meridian_cities_indexes.insert({city_id, i});
    }

    int from, to, price;

    for (int i = 0; i < road_count; i++) {
        scan_int(&from);
        scan_int(&to);
        scan_int(&price);

        from--;
        to--;

        if (!cities[to]->zeroth_meridian_city) {
            cities[from]->sort_neighbours.push_back(to);
        }
        cities[from]->neighbours.emplace_back(to, price);
        cities[to]->to_road_count++;
    }
}

void topological_sort() {
    std::queue<struct city *> sort_queue;

    for (const auto &city : zeroth_meridian_cities) {
        sort_queue.push(cities[city]);
    }

    while(!sort_queue.empty()) {
        struct city* top = sort_queue.front();
        sort_queue.pop();
        topological_sorted_cities.push_back(top);

        for (const auto &city : top->sort_neighbours) {
            struct city* neighbour = cities[city];
            neighbour->to_road_count--;

            if (neighbour->to_road_count == 0) {
                sort_queue.push(neighbour);
            }
        }
    }
}

void find_cheapest_path(const int origin) {
    for (const auto &path : cities[origin]->neighbours) {
        cities[path.first]->cheapest_price_from.first = origin;
        cities[path.first]->cheapest_price_from.second = path.second;
    }

    for (auto city : topological_sorted_cities) {
        if (city->cheapest_price_from.first == origin) {
            for (const auto &path : cities[city->id]->neighbours) {
                if (cities[path.first]->cheapest_price_from.first == origin) {
                    if (city->cheapest_price_from.second + path.second < cities[path.first]->cheapest_price_from.second) {
                        cities[path.first]->cheapest_price_from.second = city->cheapest_price_from.second + path.second;
                        //printf("graph[%d]->curr_min_distance = %d\n", path.first, cities[path.first]->cheapest_price_from.second);
                    }
                } else {
                    cities[path.first]->cheapest_price_from.second = city->cheapest_price_from.second + path.second;
                    //printf("graph[%d]->curr_min_distance = %d\n", path.first, cities[path.first]->cheapest_price_from.second);
                    cities[path.first]->cheapest_price_from.first = origin;
                    //printf("graph[%d]->search = %d\n", path.first, cities[path.first]->cheapest_price_from.first);
                }
            }
        }
    }
}

void find_cheapest_paths() {
    for (const auto &city_origin : zeroth_meridian_cities) {
        find_cheapest_path(city_origin);

        for (const auto &city_destination : zeroth_meridian_cities) {
            if (cities[city_destination]->cheapest_price_from.first == city_origin) {
                cheapest_path[zeroth_meridian_cities_indexes[city_origin]][zeroth_meridian_cities_indexes[city_destination]] = cities[city_destination]->cheapest_price_from.second;
            }
        }
    }
}

void find_optimal_price(int circulation_remaining, int current_price, int origin) {
    if (circulation_remaining == 0) {
        if (current_price < optimal_price) {
            optimal_price = current_price;
        }
        return;
    }

    for (int i = 0; i < zeroth_meridian_city_count; i++) {
        int price = cheapest_path[origin][i];

        if (price > 0 && current_optimal[i][circulation_remaining - 1] > current_price + price) {
            current_optimal[i][circulation_remaining - 1] = current_price + price;
            find_optimal_price(circulation_remaining - 1, current_price + price, i);
        }
    }
}

void recursion() {
    for (int i = 0; i < zeroth_meridian_city_count; i++) {
        find_optimal_price(circulation_count, 0, i);
    }
}

void cleanse_memory() {
    for (auto &city : cities) {
        delete city;
    }
}

int main() {
    load_input();
    topological_sort();
    find_cheapest_paths();
    recursion();
    printf("%d\n", optimal_price);
    cleanse_memory();
    return EXIT_SUCCESS;
}
