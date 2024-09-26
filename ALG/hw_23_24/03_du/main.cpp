#include "includes.h"

int main()
{
    input_t in;
    read_input(&in);

    in.root = in.tree_array[0];
    in.root = new_tree(in.n, in.tree_array, in.values);

    printf("%d\n", in.root->cities);

    return 0;
}
