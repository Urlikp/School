//
// Created by vojte on 03.01.2024.
//

#include "utils.h"

input_t read_input()
{
    input_t in;

    scanf("%d %d %d %d", &in.n, &in.k, &in.l, &in.s);

    in.map = (int *)malloc((in.n * in.n) * sizeof(int));

    for (int i = 0; i < in.n * in.n; ++i)
    {
        scanf("%d", &in.map[i]);
    }

    // Calculate Center
    in.c = in.k - 2 * in.l;

    return in;
}

void print_2d_array(int *m, int n)
{
    for (int i = 0; i < n * n; ++i)
    {
        printf("%d ", m[i]);
        if ((i + 1) % n == 0)
            printf("\n");
    }
}

void print_input(input_t in)
{
    printf("Input:\nN=%d K=%d L=%d S=%d C=%d\n", in.n, in.k, in.l, in.s, in.c);

    print_2d_array(in.map, in.n);
}

void print_suitable_locs(queue<pair<int, int>> suitable_locations)
{

    // DEBUG
    printf("Founded suitable locations on: ");
    while (!suitable_locations.empty())
    {
        std::cout << suitable_locations.front().first << " " << suitable_locations.front().second
                  << " || ";      // Print the front element
        suitable_locations.pop(); // Remove the front element
    }
}