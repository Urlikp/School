//
// Created by vojte on 03.01.2024.
//

#include "algorithm.h"

// Function to read input
void calc_prefix_sum(input_t in, prefixes_t *pref)
{
    // Allocating memory for prefix sum
    int *prefix_sum = (int *)malloc((in.n * in.n) * sizeof(int));

    pref->rocks = (int *)malloc((in.n * in.n) * sizeof(int));
    pref->trees = (int *)malloc((in.n * in.n) * sizeof(int));

    // Calculating prefix sum
    // For each row
    for (int row = 0; row < in.n; row++)
    {
        // Calculating prefix sum for each column
        for (int col = 0; col < in.n; col++)
        {
            // Set index
            int index = row * in.n + col;

            // * TREES
            // Set value to be placed in map
            int value_in_map_trees = 0;
            // If in map is the opposite of what I want then set value to 0, otherwise set value to what is in map
            // That means 0 or what I am looking for
            if (in.map[index] != 2)
                value_in_map_trees = in.map[index];

            // Set value in prefix sum
            pref->trees[index] = value_in_map_trees;

            // * ROCKS
            // Set value to be placed in map
            int value_in_map_rocks = 0;
            // If in map is the opposite of what I want then set value to 0, otherwise set value to what is in map
            // That means 0 or what I am looking for
            if (in.map[index] != 1)
                value_in_map_rocks = in.map[index];

            // Set value in prefix sum
            pref->rocks[index] = value_in_map_rocks;

            // If row is greater than 0 then add the value from the top
            // That means I am not adding anything outside the map
            if (row > 0)
            {
                pref->trees[index] += pref->trees[index - in.n];
                pref->rocks[index] += pref->rocks[index - in.n];
            }

            // If col is greater than 0 then add the value from the left
            // That means I am not adding anything outside the map
            if (col > 0)
            {
                pref->trees[index] += pref->trees[index - 1];
                pref->rocks[index] += pref->rocks[index - 1];
            }

            // If both row and col are greater than 0 then subtract the value from the top-left
            // Otherwise its just row or col
            if (row > 0 && col > 0)
            {
                pref->trees[index] -= pref->trees[index - in.n - 1];
                pref->rocks[index] -= pref->rocks[index - in.n - 1];
            }
        }
    }
    // Return prefix sum
}

// Function to calculate sum in prefix sum
int calc_sum_in_pref(const int *prefix_sum, int n, int up_row, int up_col, int down_row, int down_col)
{
    // Set sum to be the value in the bottom right corner
    int sum = prefix_sum[down_row * n + down_col]; // Sum of the entire rectangle

    // If up_row is greater than 0 then subtract the top part
    // That means I am not subtracting anything outside the map
    if (up_row > 0)
    {
        sum -= prefix_sum[(up_row - 1) * n + down_col]; // Subtract the top part
    }
    // If up_col is greater than 0 then subtract the left part
    // That means I am not subtracting anything outside the map
    if (up_col > 0)
    {
        sum -= prefix_sum[down_row * n + (up_col - 1)]; // Subtract the left part
    }

    // If both up_row and up_col are greater than 0 then add back the top-left part
    if (up_row > 0 && up_col > 0)
    {
        sum += prefix_sum[(up_row - 1) * n + (up_col - 1)]; // Add back the top-left part
    }

    return sum; // Return the answer
}

// Function to print 2d array
int suitable_locs(input_t in, prefixes_t pref)
{
    // Size of park
    // Calculated by subtracting the size of the park and center from the size of the map
    int size_of_park = in.n - in.l - in.c;

    int max = 0;

    // For each row in inner part of whole map
    for (int row = in.l; row <= size_of_park; ++row)
    {
        // For each column in inner part of whole map
        for (int col = in.l; col <= size_of_park; ++col)
        {
            // If sum of rocks in rectangle is greater than or equal to half of the area of the rectangle
            // Sum / ROCK = number of rocks in area
            if (calc_sum_in_pref(pref.rocks, in.n, row, col, row + in.c - 1, col + in.c - 1) / 2 >= in.s)
            {
                // Defining area of whole park
                int up_row = row - in.l;
                int up_col = col - in.l;
                int down_row = up_row + in.k - 1;
                int down_col = up_col + in.k - 1;

                // Calculating sum of trees in park
                int sum = calc_sum_in_pref(pref.trees, in.n, up_row, up_col, down_row, down_col);
                // Control with maximum
                if (sum > max)
                {
                    max = sum;
                }
            }
        }
    }
    return max;
}
