//
// Created by vojte on 11.11.2023.
//

#include "algorithm.h"

// Define possible moves
int moves[4][2] = {{-1, 0},
                   {0, 1},
                   {1, 0},
                   {0, -1}};

bool is_valid(input_t *input, position_t pos, int color_key)
{
    // If not in map, return false
    if (!in_map(input, pos))
    {
        return false;
    }

    // Not being a wall
    int current_pos = input->map[pos.row * input->n + pos.col];
    // Go through all walls, if it's not the color key, add it to the current pos
    for (int i = 0; i < input->c; ++i)
    {
        if (i != color_key)
        {
            current_pos += input->color_map[i][pos.row * input->n + pos.col];
        }

        if (current_pos > 0)
        {
            return false;
        }
    }
    return true;
}

// Function to return the distance between two pos. Return -1 if not possible
int shortest_path(input_t *input)
{
    // Check if the start and end pos are not the same
    if (input->start.row == input->end.row && input->start.col == input->end.col)
    {
        return SAME;
    }

    // BFS algorithm

    // Define queue
    std::queue<cell_t> q;
    // Add the start pos to the queue
    cell_t start_position = {input->start, 0, ALL_UP};
    q.push(start_position);
    int index_start = input->start.row * input->n + input->start.col;
    input->color_visited[index_start][ALL_UP] = true;

    while (!q.empty())
    {
        // Get the first element from the queue
        cell_t curr = q.front();
        q.pop();

        // Check if the current pos is the end pos
        // Validni protoze BFS hleda nejkratsi cestu
        if (curr.pos.row == input->end.row && curr.pos.col == input->end.col)
        {
            return curr.distance;
        }

        // Check if the current pos is a button
        // If it is, change the color key that will be passed to the next pos
        int my_pos = input->map[curr.pos.row * input->n + curr.pos.col];
        if (my_pos < 0 && curr.color_key != std::abs(my_pos) - 1)
        {
            // ! Change the color
            curr.color_key = std::abs(my_pos) - 1; // -1 because of 0 indexing
        }

        // Check all possible moves
        for (auto &move : moves)
        {
            // Set next pos
            position_t next{curr.pos.row + move[0], curr.pos.col + move[1]};
            int next_index = next.row * input->n + next.col;

            // Check if the next pos is valid and not visited
            if (is_valid(input, next, curr.color_key) && !input->color_visited[next_index][curr.color_key])
            {
                // Add the new pos to the queue
                cell_t new_position = {next, curr.distance + 1, curr.color_key};
                q.push(new_position);
                input->color_visited[next_index][curr.color_key] = true;
            }
        }
    }

    return NOT_FOUND;
}