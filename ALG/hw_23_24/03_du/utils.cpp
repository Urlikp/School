//
// Created by vojte on 03.01.2024.
//

#include "utils.h"

// Function to create a new node
node_t *create_node(int id)
{
    node_t *node = new node_t;

    node->id = id;
    node->left = nullptr;
    node->right = nullptr;

    return node;
}

// Function to read the input
void read_input(input_t *in)
{
    scanf("%d", &in->n);

    in->tree_array = (node_t **)malloc(sizeof(node_t *) * in->n);

    in->values = (int *)malloc(sizeof(int) * in->n);

    for (int i = 0; i < in->n; i++)
    {
        int value;
        scanf("%d", &value);
        in->values[i] = value;

        in->tree_array[i] = create_node(i + 1);
    }
}