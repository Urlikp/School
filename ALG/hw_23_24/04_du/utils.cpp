//
// Created by vojte on 11.11.2023.
//
#include "utils.h"

void read_input(input_t *input)
{
    // First line
    scanf("%d %d %d", &input->m, &input->n, &input->c);

    input->start = {input->m - 1, 0};
    input->end = {0, input->n - 1};

    // Second line and on
    input->map = (int *)malloc((input->m * input->n) * sizeof(int));

    // Create array of arrays for colors
    input->color_map = (int **)malloc(input->c * sizeof(int *));

    // Creat bool array for each color
    input->color_visited = (bool **)malloc((input->m * input->n) * sizeof(bool *));

    // Set color_map to false
    for (int i = 0; i < input->c; ++i)
    {
        input->color_map[i] = (int *)calloc(input->m * input->n, sizeof(int));
    }

    for (int i = 0; i < input->m * input->n; i++)
    {
        // Initialize color_visited array, c+1 because of default map
        input->color_visited[i] = (bool *)calloc(input->c + 1, sizeof(bool));

        int current;
        scanf("%d", &current);
        input->map[i] = 0;
        if (current > 0)
        { // Not handling buttons with value below 0
            input->color_map[current - 1][i] = 1;
            continue;
        }
        input->map[i] = current;
    }
}

void print_input(input_t *input)
{
    printf("%d %d %d\n", input->m, input->n, input->c);

    // Clear map
    for (int row = 0; row < input->m; ++row)
    {
        for (int col = 0; col < input->n; ++col)
        {
            int index = row * input->n + col;
            printf("%d\t", input->map[index]);
        }
        printf("\n");
    }
    // Color maps
    for (int i = 0; i < input->c; ++i)
    {
        printf("Color %d\n", i + 1);
        for (int row = 0; row < input->m; ++row)
        {
            for (int col = 0; col < input->n; ++col)
            {
                int index = row * input->n + col;
                printf("%d\t", input->color_map[i][index]);
            }
            printf("\n");
        }
    }
}

bool in_map(input_t *input, position_t pos)
{
    return !(pos.row < 0 || pos.row >= input->m || pos.col < 0 || pos.col >= input->n);
}