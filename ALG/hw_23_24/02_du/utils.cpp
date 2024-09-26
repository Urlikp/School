//
// Created by vojte on 03.01.2024.
//

#include "utils.h"

// Returns the depth of the tree
int depth(node_t *node)
{
    if (node == nullptr)
        return -1;
    return 1 + max(depth(node->left), depth(node->right));
}

// Returns the number of nodes in the tree
int count(node_t *node)
{
    if (node == nullptr)
        return 0;
    return 1 + count(node->left) + count(node->right);
}

node_t *create_node(int id)
{
    auto *node = new node_t;
    node->id = id;
    node->right = nullptr;
    node->left = nullptr;

    node->parent_id = -1;

    node->time_to_get_here = 0;

    return node;
}

void read_input(input_t *input, algorithm_t *algorithm)
{
    // First line
    scanf("%d %d", &input->n, &input->b);

    // Second line
    input->boxes = (int *)malloc((input->b) * sizeof(int));
    for (int i = 0; i < input->b; ++i)
    {
        scanf("%d", &input->boxes[i]);
        algorithm->balanc_max += input->boxes[i];
    }

    // Third line and on
    input->tree_array = (node_t **)calloc(input->n, sizeof(node_t *));

    for (int i = 0; i < input->n - 1; i++)
    {
        int parent_id, child_id, cost;
        scanf("%d %d %d", &parent_id, &child_id, &cost);

        if (input->tree_array[parent_id] == nullptr)
        {
            input->tree_array[parent_id] = create_node(parent_id);
        }

        node_t *curr = input->tree_array[parent_id];

        curr->id = parent_id;

        if (curr->left == nullptr)
        {
            input->tree_array[child_id] = create_node(child_id);
            curr->left = input->tree_array[child_id];
            curr->left->time_to_get_here = curr->time_to_get_here + cost;
            curr->left->parent_id = parent_id;
        }
        else
        {
            input->tree_array[child_id] = create_node(child_id);
            curr->right = input->tree_array[child_id];
            curr->right->time_to_get_here = curr->time_to_get_here + cost;
            curr->right->parent_id = parent_id;
        }
    }
}
