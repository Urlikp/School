#include <cstdio>
#include <algorithm>
#include <vector>
#include <queue>

#define NOT_VISITED 0
#define NO_MARK     0

#define SCAN_INT

struct node {
    const int row;
    const int column;
    int visited = NOT_VISITED;
    int mark = NO_MARK;
    int distance = std::numeric_limits<int>::max();

    node(const int r, const int c) : row(r), column(c) {
    }
};

int row_count, column_count, obstacle_count, mark_type_count, minimum = 0;
std::vector<std::vector<node*>> nodes;
int change_x[4] = {1, 0, -1, 0}, change_y[4] = {0, 1, 0, -1};
struct node* source;

inline void scan_int(int *x) {
    char c = (char)getchar_unlocked();
    *x = 0;
    while (c < 48 || c > 57) {
        c = (char)getchar_unlocked();
    }

    while (c > 47 && c < 58) {
        *x = (int)((((*x) << 1) + ((*x) << 3)) + c - 48);
        c = (char)getchar_unlocked();
    }
}

void load_graph() {
#ifdef SCAN_INT
    scan_int(&row_count);
    scan_int(&column_count);
    scan_int(&obstacle_count);
    scan_int(&mark_type_count);
#else
    if (scanf("%d %d %d %d", &row_count, &column_count, &obstacle_count, &mark_type_count) != 4) {
        fprintf(stderr, "Error: Wrong input!\n");
        exit(EXIT_FAILURE);
    }
#endif

    try {
        nodes = std::vector<std::vector<node*>>(row_count, std::vector<node*>(column_count));

        for (int i = 0; i < row_count; i++) {
            for (int j = 0; j < column_count; j++) {
                nodes[i][j] = new struct node(i, j);
            }
        }
    } catch (std::bad_alloc& ex) {
        fprintf(stderr, "Error: Bad allocation!\n");
        exit(EXIT_FAILURE);
    }

    int row, col;

#ifdef SCAN_INT
    scan_int(&row);
    scan_int(&col);
#else
    if (scanf("%d %d", &row, &col) != 2) {
        fprintf(stderr, "Error: Wrong input!\n");
        exit(EXIT_FAILURE);
    }
#endif

    row = abs(row - row_count);
    source = nodes[row][col - 1];
    source->visited = 1;

    for (int i = 0; i < obstacle_count; i++) {
#ifdef SCAN_INT
        scan_int(&row);
        scan_int(&col);
#else
        if (scanf("%d %d", &row, &col) != 2) {
            fprintf(stderr, "Error: Wrong input!\n");
            exit(EXIT_FAILURE);
        }
#endif

        row = abs(row - row_count);
        nodes[row][col - 1]->visited = mark_type_count + 10;
    }

    int mark_count;

    for (int i = 0; i < mark_type_count; i++) {
#ifdef SCAN_INT
        scan_int(&mark_count);
#else
        if (scanf("%d", &mark_count) != 1) {
            fprintf(stderr, "Error: Wrong input!\n");
            exit(EXIT_FAILURE);
        }
#endif

        for (int j = 0; j < mark_count; j++) {
#ifdef SCAN_INT
            scan_int(&row);
            scan_int(&col);
#else
            if (scanf("%d %d", &row, &col) != 2) {
                fprintf(stderr, "Error: Wrong input!\n");
                exit(EXIT_FAILURE);
            }
#endif
            row = abs(row - row_count);
            nodes[row][col - 1]->mark = i + 1;
        }
    }
}

bool in_bounds(int x, int y) {
    return (x >= 0 && y >= 0) && (x < row_count && y < column_count);
}

int find_distance() {
    std::queue<node*> queue;
    queue.push(source);
    source->visited = 1;

    int distance = 0, a = 1, b = 0, shortest_path = std::numeric_limits<int>::max();
    int x, y, x_new, y_new;

    while (!queue.empty()) {
        node* current = queue.front();
        queue.pop();
        a--;

        x = current->row;
        y = current->column;

        for (size_t i = 0; i < 4; i++) {
            x_new = x + change_x[i];
            y_new = y + change_y[i];

            if (in_bounds(x_new, y_new)) {
                node* new_node = nodes[x_new][y_new];

                if (new_node->visited == NOT_VISITED) {
                    new_node->visited = 1;
                    if (new_node->mark == 1) {
                        new_node->visited = 2;
                        new_node->distance = distance + 1;
                    }

                    queue.push(new_node);
                    b++;
                }

                if (current->visited > 1) {
                    if (new_node->mark == current->visited) {
                        if (new_node->mark == mark_type_count) {
                            if (current->distance + 1 < shortest_path) {
                                shortest_path = current->distance + 1;
                            }
                        } else {
                            if (new_node->visited < new_node->mark + 1) {
                                new_node->visited = current->visited + 1;
                                new_node->distance = current->distance + 1;
                                queue.push(new_node);
                                b++;
                            } else if (new_node->visited == new_node->mark + 1) {
                                if (new_node->distance > current->distance + 1) {
                                    new_node->visited = current->visited + 1;
                                    new_node->distance = current->distance + 1;
                                    queue.push(new_node);
                                    b++;
                                }
                            }
                        }
                    } else {

                        if (new_node->visited < current->visited) {
                            new_node->visited = current->visited;
                            new_node->distance = current->distance + 1;
                            queue.push(new_node);
                            b++;
                        } else if (new_node->visited == current->visited) {
                            if (new_node->distance > current->distance + 1) {
                                new_node->visited = current->visited;
                                new_node->distance = current->distance + 1;
                                queue.push(new_node);
                                b++;
                            }
                        }
                    }
                }
            }
        }

        if (a == 0) {
            a = b;
            b = 0;
            distance++;
        }
    }

    return shortest_path;
}

int main() {
    load_graph();
    minimum = find_distance();
    for (int i = 0; i < row_count; i++) {
        for (int j = 0; j < column_count; j++) {
            delete nodes[i][j];
        }
    }
    printf("%d\n", minimum);
    return EXIT_SUCCESS;
}
