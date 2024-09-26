//
// Created by vojte on 03.01.2024.
//

#include "algorithm.h"

// Function to calculate the roads under a node
void calculate_roads(node_t *node)
{
    // If the node is a leaf, set its values to zero
    if (node->left == nullptr && node->right == nullptr)
    {
        node->cities = 1;
        node->depth = 0;
        return;
    }
    else if (node->left == nullptr)
    {
        node->cities = node->right->cities + 1;
        node->depth = node->right->depth + 1;
        return;
    }
    else if (node->right == nullptr)
    {
        node->depth = node->left->depth + 1;
        node->cities = node->left->cities + 1;
        return;
    }

    node->depth = max(node->left->depth, node->right->depth) + 1;

    int left_max = node->left->cities + (node->right->depth + 1) + 1;
    int right_max = node->right->cities + (node->left->depth + 1) + 1;

    // Compare the number of cities
    if (left_max > right_max)
    {
        node->cities = left_max;
    }
    else
    {
        node->cities = right_max;
    }
}

// Function to create a new tree
node_t *new_tree(int n, node_t *tree_arr[], int *values)
{
    if (n == 0)
        return nullptr;

    node_t *root = tree_arr[values[0] - 1];

    int *left_arr = (int *)malloc(sizeof(int) * (n - 1));
    int *right_arr = (int *)malloc(sizeof(int) * (n - 1));

    int left_count = 0, right_count = 0;

    // Split the array into two parts
    for (int i = 1; i < n; ++i)
    {
        int current = values[i];
        if (current < root->id)
            left_arr[left_count++] = values[i];
        else
            right_arr[right_count++] = values[i];
    }

    root->left = new_tree(left_count, tree_arr, left_arr);
    root->right = new_tree(right_count, tree_arr, right_arr);

    calculate_roads(root);

    return root;
}
