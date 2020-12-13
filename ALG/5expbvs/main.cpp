#include <cstdio>
#include <algorithm>
#include <vector>
#include <iostream>

#define ADD         1
#define SUBTRACT    -1
#define SPACE_COUNT 10
//#define PRINT

struct node {
    int id;
    int left_subtree_size = 0, right_subtree_size = 0;
    struct node* parent = nullptr;
    struct node* left = nullptr;
    struct node* right = nullptr;
public:
    explicit node(const int id) : id(id) {
    }
};

std::vector<int> sorted_nodes;
struct node *root = nullptr, *found = nullptr;
bool left = false, balanced = false;

void inorder_print(node* ptr) {
    if (ptr == nullptr) {
        return;
    }
    inorder_print(ptr->left);
    printf("%d ", ptr->id);
    inorder_print(ptr->right);

}

void print(node* ptr, int space) {
    if (ptr == nullptr) {
        return;
    }

    space += SPACE_COUNT;

    print(ptr->right, space);

    std::cout << std::endl;
    for (int i = SPACE_COUNT; i < space; i++) {
        std::cout << " ";
    }
    std::cout << ptr->id << std::endl;

    print(ptr->left, space);
}

void destroy_subtree(node** ptr) {
    if ((*ptr) == nullptr) {
        return;
    }

    destroy_subtree(&((*ptr)->left));
    destroy_subtree(&((*ptr)->right));
    delete (*ptr);
    (*ptr) = nullptr;
}

void inorder_to_array(node* ptr) {
    if (ptr == nullptr) {
        return;
    }

    inorder_to_array(ptr->left);
    sorted_nodes.push_back(ptr->id);
    inorder_to_array(ptr->right);
}

int log_2(int number) {
    int power = 1;
    while (power < number) {
        power <<= 1;
    }

    if (power <= number) {
        return power;
    }

    return power >> 1;
}

int depth_of_tree(node* ptr) {
    if (ptr == nullptr)   {
        return 0;
    } else  {
        int left_depth = depth_of_tree(ptr->left);
        int right_depth = depth_of_tree(ptr->right);

        if (left_depth > right_depth) {
            return(left_depth + 1);
        } else  {
            return(right_depth + 1);
        }
    }
}

node* sorted_array_to_BST(int start, int end, node* parent)  {
    if (start >= end) {
        return nullptr;
    }

    int array_size = end - start;
    int M = log_2(array_size);
    int R = (array_size) - (M - 1);
    int LT, RT;
    if (R <= (M/2)) {
        LT = ((M - 2)/2) + R;
        RT = (M - 2)/2;
    } else {
        LT = ((M - 2)/2) + (M/2);
        RT = ((M - 2)/2) + R - (M/2);
    }

    node* subtree_root = new struct node(sorted_nodes[start + LT]);
    subtree_root->left_subtree_size = LT;
    subtree_root->right_subtree_size = RT;
    subtree_root->parent = parent;

    if (LT == 1) {
        subtree_root->left = new struct node(sorted_nodes[start + LT - 1]);
        subtree_root->left->parent = subtree_root;
    } else {
        subtree_root->left = sorted_array_to_BST(start, start + LT, subtree_root);
    }

    if (RT == 1) {
        subtree_root->right = new struct node(sorted_nodes[start + LT + 1]);;
        subtree_root->right->parent = subtree_root;
    } else {
        subtree_root->right = sorted_array_to_BST(end - RT, end, subtree_root);
    }

    return subtree_root;
}

node* recursion_subtree_min_node(node* node) {
    struct node* current = node;

    while (current && current->left != nullptr) {
        current = current->left;
    }
    return current;
}

int subtree_min_node(node* node) {
    struct node* ptr = node;
    bool delete_parent = true;

    while (ptr && ptr->left) {
        ptr->left_subtree_size--;
        ptr = ptr->left;
        delete_parent = false;
    }

    int id = ptr->id;

    if (delete_parent) {
        ptr->parent->right = nullptr;
    } else {
        ptr->parent->left = nullptr;
    }

    if (ptr->right != nullptr) {
        ptr->parent->left = ptr->right;
        ptr->right->parent = ptr->parent;
    }

    delete ptr;

    return id;
}

void change_parent_subtree_sizes(node* node, const int id, const int change) {
    struct node* ptr = node;

    while(ptr != nullptr) {
        if (id < ptr->id) {
            ptr->left_subtree_size += change;
        } else {
            ptr->right_subtree_size += change;
        }

        ptr = ptr->parent;
    }
}

struct node* balance(node* ptr) {
    int cond_left_side = abs(ptr->left_subtree_size - ptr->right_subtree_size);
    int cond_right_side = ((std::max(ptr->left_subtree_size, ptr->right_subtree_size) + 1) / 2 );

    if (cond_left_side > cond_right_side) {
        balanced = true;
        struct node* parent = ptr->parent;
        inorder_to_array(ptr);
        destroy_subtree(&ptr);
        ptr = sorted_array_to_BST(0, sorted_nodes.size(), parent);

        if (ptr->parent != nullptr) {
            if (ptr->id < ptr->parent->id) {
                ptr->parent->left = ptr;
            } else {
                ptr->parent->right = ptr;
            }
        } else {
            root = ptr;
        }

        sorted_nodes.clear();
#ifdef PRINT
        print(root, 0);
        printf("------------------------BALANC------------------------\n");
#endif
    }


    return ptr;
}

bool contains(const int id) {
    node* ptr = root;
    found = ptr;

    while (ptr != nullptr) {
        found = ptr;

        if (id < ptr->id) {
            ptr = ptr->left;
            left = true;
        } else if (id > ptr->id) {
            ptr = ptr->right;
            left = false;
        } else {
            return true;
        }
    }

    return false;
}

void insert(const int id) {
    node* ptr = found;

    if (id == 4497 || id == 7535 || id == 7536) {
        bool problem = true;
        problem = !problem;
    }

    if (ptr == nullptr) {
        root = new struct node(id);
        return;
    }

    change_parent_subtree_sizes(ptr, id, ADD);

    if(left) {
        ptr->left = new struct node(id);
        ptr->left->parent = ptr;
    } else {
        ptr->right = new struct node(id);
        ptr->right->parent = ptr;
    }
}

node* recursion_remove(node** ptr, const int id) {
    if ((*ptr) == nullptr) {
        return (*ptr);
    }

    if (id < (*ptr)->id) {
        (*ptr)->left_subtree_size--;

        (*ptr)->left = recursion_remove(&((*ptr)->left), id);

        /*if ((*ptr)->left != nullptr) {
            (*ptr)->left->parent = (*ptr);
        }*/
    } else if (id > (*ptr)->id) {
        (*ptr)->right_subtree_size--;

        (*ptr)->right = recursion_remove(&((*ptr)->right), id);
        /*if ((*ptr)->right != nullptr) {
            (*ptr)->right->parent = (*ptr);
        }*/
    } else {
        if ((*ptr)->left == nullptr) {
            if ((*ptr)->right != nullptr) {
                (*ptr)->right->parent = ((*ptr)->parent);
            }

            node* temp = (*ptr)->right;
            delete (*ptr);
            (*ptr) = nullptr;
            return temp;
        } else if ((*ptr)->right == nullptr) {
            if ((*ptr)->left != nullptr) {
                (*ptr)->left->parent = ((*ptr)->parent);
            }

            node* temp = (*ptr)->left;
            delete (*ptr);
            (*ptr) = nullptr;
            return temp;
        }

        node* temp = recursion_subtree_min_node((*ptr)->right);
        (*ptr)->id = temp->id;
        (*ptr)->right_subtree_size--;

        (*ptr)->right = recursion_remove(&((*ptr)->right), temp->id);
        /*if ((*ptr)->right != nullptr) {
            (*ptr)->right->parent = (*ptr);
        }*/
    }
    return (*ptr);
}

void remove(const int id) {
    node* ptr = found;

    if (id == 4615) {
        bool problem = true;
        problem = !problem;
    }

    if (ptr->left == nullptr) {
        if (ptr->right != nullptr) {
            ptr->right->parent = ptr->parent;
        }

        if (ptr->parent != nullptr && ptr->id < ptr->parent->id) {
            ptr->parent->left = ptr->right;
        } else if (ptr->parent != nullptr) {
            ptr->parent->right = ptr->right;
        }

        if (ptr->parent == nullptr) {
            root = ptr->right;
        }
    } else if (ptr->right == nullptr) {
        if (ptr->left != nullptr) {
            ptr->left->parent = ptr->parent;
        }

        if (ptr->parent != nullptr && ptr->id < ptr->parent->id) {
            ptr->parent->left = ptr->left;
        } else if (ptr->parent != nullptr) {
            ptr->parent->right = ptr->left;
        }

        if (ptr->parent == nullptr) {
            root = ptr->left;
        }
    } else {
        struct node* successor = new struct node(subtree_min_node(ptr->right));

        successor->parent = ptr->parent;

        ptr->left->parent = successor;
        if(ptr->right != nullptr) {
            ptr->right->parent = successor;
        }

        successor->left = ptr->left;
        successor->right = ptr->right;

        successor->left_subtree_size = ptr->left_subtree_size;
        successor->right_subtree_size = ptr->right_subtree_size - 1;

        if (ptr->parent != nullptr && ptr->id < ptr->parent->id) {
            ptr->parent->left = successor;
        } else if (ptr->parent != nullptr) {
            ptr->parent->right = successor;
        }

        if (ptr->parent == nullptr) {
            root = successor;
        }
    }

    change_parent_subtree_sizes(ptr->parent, id, SUBTRACT);
    delete ptr;
}

void load_graph() {
    size_t operation_count;
    int input;

    if (scanf("%lu", &operation_count) != 1) {
        fprintf(stderr, "Error: Wrong input!\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < operation_count; i++) {
        if (scanf("%d", &input) != 1) {
            fprintf(stderr, "Error: Wrong input!\n");
            exit(EXIT_FAILURE);
        }

        if (input == -4615) {
            bool problem = true;
            problem = !problem;
        }

        if (input > 0 && !contains(input)) {
            insert(input);
        } else if (input < 0 && contains(-input)) {
            //remove(-input);
            root = recursion_remove(&root, -input);
        }
#ifdef PRINT
        print(root, 0);
        printf("------------------------CHANGE------------------------\n");
#endif
        int id = abs(input);

        struct node* ptr = root;
        while (ptr != nullptr && !balanced) {
            if (id < ptr->id) {
                ptr = balance(ptr);
                ptr = ptr->left;
            } else if (id > ptr->id) {
                ptr = balance(ptr);
                ptr = ptr->right;
            } else {
                ptr = balance(ptr);
                break;
            }
        }
        balanced = false;
    }
}

int main() {
    load_graph();
    if (root) {
#ifdef PRINT
        inorder_print(root);
        printf("\n");
#endif
        printf("%d %d %d\n", depth_of_tree(root) - 1, root->left_subtree_size, root->right_subtree_size);
        destroy_subtree(&root);
    } else {
        printf("0 0 0\n");
    }

    return EXIT_SUCCESS;
}
