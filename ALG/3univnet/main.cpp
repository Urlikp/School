#include <cstdio>
#include <algorithm>
#include <vector>

#define ROUTER      0
#define EXTRACTOR   1
#define VALIDATOR   2

struct node {
    int id = 0;
    int type = ROUTER;
    std::vector<int> connections;
    int edge_count = 0;
    struct node* left = nullptr;
    struct node* right = nullptr;
    //struct node* parent = nullptr;
public:
    explicit node(int id) {
        this->id = id;
    }
};

int node_count, total_edge_count, extractor_count, validator_count, maximum = 0;
std::vector<node*> nodes;
struct node* root;

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
    if (scanf("%d %d %d %d", &node_count, &total_edge_count, &extractor_count, &validator_count) != 4) {
        fprintf(stderr, "Error: Wrong input!\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < node_count; i++) {
        try {
            nodes.push_back(new node(i));
        } catch(std::bad_alloc& ex) {
            fprintf(stderr, "Error: Bad allocation!\n");
            exit(EXIT_FAILURE);
        }
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

    for (int i = 0; i < extractor_count; i++) {
        if (scanf("%d", &j) != 1) {
            fprintf(stderr, "Error: Wrong input!\n");
            exit(EXIT_FAILURE);
        }
        nodes[j-1]->type = EXTRACTOR;
    }

    for (int i = 0; i < validator_count; i++) {
        if (scanf("%d", &j) != 1) {
            fprintf(stderr, "Error: Wrong input!\n");
            exit(EXIT_FAILURE);
        }
        nodes[j-1]->type = VALIDATOR;
    }
}

void add_node(struct node* current, struct node* parent)
{
    //current->parent = parent;

    if (current->edge_count == 1)
    {
        current->left = nullptr;
        current->right = nullptr;
    }
    else
    {
        if (nodes[current->connections[0]] == parent) {
            current->left = nodes[current->connections[1]];
        } else {
            current->left = nodes[current->connections[0]];
        }

        add_node(current->left, current);

        if (current->edge_count == 2)
        {
            current->right = nullptr;
        }
        else
        {
            if (nodes[current->connections[2]] == parent) {
                current->right = nodes[current->connections[1]];
            } else {
                current->right = nodes[current->connections[2]];
            }

            add_node(current->right, current);
        }
    }
}

void create_tree() {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]->edge_count == 2) {
            root = nodes[i];
            break;
        }
    }

    root->left = nodes[root->connections[0]];
    root->right = nodes[root->connections[1]];
    add_node(root->left, root);
    add_node(root->right, root);
}

int find_route(struct node* current) {
    int returned_left = ROUTER, returned_right = ROUTER;
    if (current->left != nullptr) {
        returned_left = find_route(current->left);
    }

    if (current->right != nullptr) {
        returned_right = find_route(current->right);
    }

    if (current->left == nullptr) {
        return current->type;
    }

    if (returned_right + returned_left == EXTRACTOR + VALIDATOR) {
        maximum++;
        return ROUTER;
    } else {
        if (returned_left != ROUTER) {
            return returned_left;
        } else {
            return returned_right;
        }
    }
}

int main() {
    load_graph();
    create_tree();
    find_route(root);
    for (int i = 0; i < node_count; i++) {
        delete nodes[i];
    }
    printf("%d\n", maximum);
    return EXIT_SUCCESS;
}
