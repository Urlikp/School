#include <cstdio>
#include <algorithm>
#include <vector>

#define EMPTY   0
#define RED     1
#define BLUE    2

struct node {
    int id = 0;
    int occupied = EMPTY;
    std::vector<int> connections;
    int edge_count = 0;
public:
    explicit node(int id) {
        this->id = id;
    }
};

int node_count, total_edge_count, red_agent_count, blue_agent_count, maximum = 0;
std::vector<node*> nodes;
std::vector<node*> sorted_nodes;

bool node_sort(const node* a, const node* b) {
    if (a->edge_count != b->edge_count) {
        return a->edge_count > b->edge_count;
    } else {
        return a->id > b->id;
    }
}

void load_graph() {
    if (scanf("%d %d %d %d", &node_count, &total_edge_count, &red_agent_count, &blue_agent_count) != 4) {
        fprintf(stderr, "Error: Wrong input!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < node_count; i++) {
        nodes.push_back(new node(i));
    }

    int j, k;
    for (int i = 0; i < total_edge_count; i++) {
        if (scanf("%d %d", &j, &k) != 2) {
            fprintf(stderr, "Error: Wrong input!\n");
            exit(EXIT_FAILURE);
        }
        nodes[j-1]->connections.push_back(k-1);
        nodes[k-1]->connections.push_back(j-1);
    }

    for (int i = 0; i < node_count; i++) {
        nodes[i]->edge_count = nodes[i]->connections.size();
    }

    sorted_nodes = nodes;
    std::sort(sorted_nodes.begin(), sorted_nodes.end(), node_sort);
}

void try_node(int red_remaining, int blue_remaining, int id, int current_score) {
    if (red_remaining + blue_remaining == 0) {
        if (current_score > maximum) {
            maximum = current_score;
        }
        return;
    }

    if (id + red_remaining + blue_remaining > node_count) {
        return;
    }

    int upper_bound = 0;
    int i;
    for (i = id; i < id + red_remaining; i++) {
        if (sorted_nodes[i]->edge_count > red_agent_count) {
            upper_bound += 2 * red_agent_count;
        } else {
            upper_bound += 2 * sorted_nodes[i]->edge_count;
        }
    }
    for (; i < id + red_remaining + blue_remaining; i++) {
        upper_bound += sorted_nodes[i]->edge_count;
    }

    if (upper_bound + current_score <= maximum) {
        return;
    }

    int temporary_score;
    if (red_remaining > 0) {
        sorted_nodes[id]->occupied = RED;
        temporary_score = current_score;
        for (auto const &j: sorted_nodes[id]->connections) {
            if (nodes[j]->occupied == RED) {
                temporary_score += 2;
            }
        }

        try_node(red_remaining - 1, blue_remaining, id + 1, temporary_score);
    }

    if (blue_remaining > 0) {
        sorted_nodes[id]->occupied = BLUE;
        temporary_score = current_score;
        for (auto const &j: sorted_nodes[id]->connections) {
            if (nodes[j]->occupied != BLUE) {
                temporary_score++;
            } else {
                temporary_score--;
            }
        }

        try_node(red_remaining, blue_remaining - 1, id + 1, temporary_score);
    }

    sorted_nodes[id]->occupied = EMPTY;
    if (id + red_remaining + blue_remaining >= node_count) {
        return;
    }

    try_node(red_remaining, blue_remaining, id + 1, current_score);

    return;
}

int main() {
    load_graph();
    try_node(red_agent_count, blue_agent_count, 0, 0);
    for (int i = 0; i < node_count; i++) {
        delete nodes[i];
    }
    printf("%d\n", maximum);
    return EXIT_SUCCESS;
}
