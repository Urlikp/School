#include <cstdio>
#include <algorithm>
#include <vector>

#define LINUX
#define BLACK 1
#define WHITE 0

int node_count, black_count = 0, optimal = 0;

struct node {
    const int id;
    const bool black;
    int left_subtree_best_black_count = 0, right_subtree_best_black_count = 0, top_subtree_best_black_count = 0;
    struct node* parent = nullptr;
    struct node* left = nullptr;
    struct node* right = nullptr;
public:
    explicit node(const int id, const bool black) : id(id), black(black) {
    }
};

struct node* root;
std::vector<struct node*> three_degree_nodes;

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


void insert(struct node **ptr, const int id, const bool black, struct node *parent) {
    if ((*ptr) == nullptr) {
        (*ptr) = new struct node(id, black);
        (*ptr)->parent = parent;
        return;
    } else if (id < (*ptr)->id) {
        insert(&((*ptr)->left), id, black, *ptr);
    } else if (id > (*ptr)->id) {
        insert(&((*ptr)->right), id, black, *ptr);
    }
}

void load_input() {
    scan_int(&node_count);

    int node_id;

    for (size_t i = 0; i < node_count; i++) {
        scan_int(&node_id);

        if (node_id > 0) {
            black_count++;
            insert(&root, node_id, true, nullptr);
        } else {
            insert(&root, -node_id, false, nullptr);
        }
    }
}

int find_three_degree(struct node *ptr) {
    if (ptr->left != nullptr && ptr->right != nullptr) {
        three_degree_nodes.push_back(ptr);

        int left = find_three_degree(ptr->left);
        int right = find_three_degree(ptr->right);

        ptr->left_subtree_best_black_count = left;
        ptr->right_subtree_best_black_count = right;

        int best = std::max(left, right);

        if (ptr->black) {
            return best + BLACK;
        }
        return best;
    } else if (ptr->left != nullptr) {
        int left = find_three_degree(ptr->left);

        ptr->left_subtree_best_black_count = left;
        ptr->right_subtree_best_black_count = WHITE;

        if (ptr->black) {
            return left + BLACK;
        }
        return left;
    } else if (ptr->right != nullptr) {
        int right = find_three_degree(ptr->right);

        ptr->right_subtree_best_black_count = right;
        ptr->left_subtree_best_black_count = WHITE;

        if (ptr->black) {
            return right + BLACK;
        }
        return right;
    } else {
        ptr->right_subtree_best_black_count = WHITE;
        ptr->left_subtree_best_black_count = WHITE;

        if (ptr->black) {
            return BLACK;
        }
        return WHITE;
    }
}

void find_top_best(struct node* ptr) {
    if (ptr == nullptr) {
        return;
    }

    int black = WHITE;
    if (ptr->black) {
        black = BLACK;
    }

    if (ptr->left != nullptr) {
        ptr->left->top_subtree_best_black_count = std::max(ptr->top_subtree_best_black_count + black, ptr->right_subtree_best_black_count + black);
        find_top_best(ptr->left);
    }

    if (ptr->right != nullptr) {
        ptr->right->top_subtree_best_black_count = std::max(ptr->top_subtree_best_black_count + black, ptr->left_subtree_best_black_count + black);
        find_top_best(ptr->right);
    }
}

void cleanse_heap(struct node** ptr) {
    if ((*ptr) != nullptr) {
        cleanse_heap(&((*ptr)->left));
        cleanse_heap(&((*ptr)->right));

        delete (*ptr);

        (*ptr) = nullptr;
    }
}

int main() {
    load_input();
    int left = 0, right = 0;
    if (root->left != nullptr) {
        left = find_three_degree(root->left);
    }

    if (root->right != nullptr) {
        right = find_three_degree(root->right);
    }

    root->left_subtree_best_black_count = left;
    root->right_subtree_best_black_count = right;
    find_top_best(root);

    int black;
    for (const auto &three_degree_node : three_degree_nodes) {
        black = WHITE;
        if (three_degree_node->black) {
            black = BLACK;
        }
        optimal = std::max(three_degree_node->left_subtree_best_black_count + three_degree_node->right_subtree_best_black_count + three_degree_node->top_subtree_best_black_count + black, optimal);
    }

    printf("%d\n", optimal);

    cleanse_heap(&root);
    return EXIT_SUCCESS;
}
