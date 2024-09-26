//
// Created by vojte on 03.01.2024.
//

#include "algorithm.h"

// Function to calculate the time of current configuration
int time_calc(input_t *input, room_t *rooms) {
    int sum = 0;
    for (int i = 0; i < input->n; ++i) {
        sum += rooms[input->tree_array[i]->id].num_of_boxes * input->tree_array[i]->time_to_get_here;
    }
    return sum;
}

// Function to calculate the balance of current configuration
int balance_calc(input_t *input, room_t *rooms) {
    int sum = 0;
    for (int i = 0; i < input->n; ++i) {
        if (input->tree_array[i]->left != nullptr)
            sum += abs(rooms[input->tree_array[i]->id].sum - rooms[input->tree_array[i]->left->id].sum);
        if (input->tree_array[i]->right != nullptr)
            sum += abs(rooms[input->tree_array[i]->id].sum - rooms[input->tree_array[i]->right->id].sum);
    }

    return sum;
}

// Function to check, whether the given node is a valid position.
// A valid position is a node when its children are not free or is a leaf
bool is_valid_pos(node_t *node, room_t *rooms) {
    // Parent of this node is already occupied
    if (node->parent_id != -1 && rooms[node->parent_id].sum != 0) {
        return false;
    }

    // If the node is a leaf node, it's always a valid position
    if (node->left == nullptr && node->right == nullptr) {
        return true;
    }

    // Node has only one child
    if (node->right == nullptr) {
        return rooms[node->left->id].sum != 0;
    }

    // One of the children is free
    if (rooms[node->left->id].sum == 0 || rooms[node->right->id].sum == 0) {
        return false;
    }

    return true;
}

void go_through(input_t *input, algorithm_t *alg, room_t *rooms, int placed_boxes, int curr_box) {
    // If all num_of_boxes are placed, do logic
    if (placed_boxes >= input->b) {
        // Logic on calculating times and balances
        int b_calc = balance_calc(input, rooms);
        if (b_calc < alg->balanc_max) {
            alg->balanc_max = b_calc;
            // Reset the time, to link balance max with time max
            alg->t_max = INT_MAX;
        }
        if (b_calc == alg->balanc_max) {
            int t_calc = time_calc(input, rooms);
            if (t_calc <= alg->t_max) {
                alg->t_max = t_calc;
            }
        }
        return;
    }

    // Create a local copy of rooms
    auto *my_rooms = (room_t *) calloc(input->n, sizeof(room_t));
    memcpy(my_rooms, rooms, input->n * sizeof(room_t));

    for (int room_id = 0; room_id < input->n; ++room_id) {
        if (is_valid_pos(input->tree_array[room_id], my_rooms)) {
            int box = input->boxes[curr_box];
            my_rooms[room_id].sum += box;
            my_rooms[room_id].num_of_boxes++;

            // I have to update that on position input->tree_array[room_id] is something placed
            go_through(input, alg, my_rooms, placed_boxes + 1, curr_box + 1);

            // Remove the box from the room
            my_rooms[room_id].sum -= box;
            my_rooms[room_id].num_of_boxes--;
        }
    }

    free(my_rooms);
}

// Function to start main alg
void algorithm(input_t *input, algorithm_t *alg) {
    // Array of rooms, where the num_of_boxes are. Calculated by the sum of the box values
    auto *rooms = (room_t *) calloc(input->n, sizeof(room_t));

    int placed_boxes = 0;
    int curr_box = 0;

    go_through(input, alg, rooms, placed_boxes, curr_box);

    free(rooms);
}
