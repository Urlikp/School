#include "includes.h"

using namespace std;

input_t *input = new input_t;

int main()
{
    read_input(input);

    // print_input(input);

    if (!in_map(input, input->start) || !in_map(input, input->end))
        return NOT_VALID;

    int path_start_end = shortest_path(input);

    if (path_start_end != NOT_FOUND)
        printf("%d\n", path_start_end);
    else
        printf("No path found\n");

    return 0;
}
