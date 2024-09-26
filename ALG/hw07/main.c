#include <stdio.h>
#include <stdlib.h>

#define MAX_R 1000
#define MAX_T 10000
#define MAX_P 1000
#define MAX_COST 1000
#define MAX_EXP 1000

typedef struct way_t
{
    int start;
    int end;
    int exp;
    int cost;
} way_t;

typedef struct signpost_t
{
    int key;
    int num_of_ways;
    way_t **ways;
    int ways_size;
} signpost_t;

typedef struct map_t
{
    int r;
    int t;
    int p;
    signpost_t **signposts;
} map_t;

way_t *create_way(int start, int end, int exp, int cost)
{
    way_t *w = malloc(sizeof(way_t));
    if (w == NULL)
        exit(1);

    w->start = start;
    w->end = end;
    w->exp = exp;
    w->cost = cost;

    return w;
}

void topo_sort(signpost_t *current_signpost, map_t *map, int *visited_arr, int *order_arr, int *order_index)
{
    visited_arr[current_signpost->key - 1] = 1;

    int num_ways = current_signpost->num_of_ways;
    way_t **ways = current_signpost->ways;
    for (int i = 0; i < num_ways; i++)
    {
        if (visited_arr[ways[i]->end - 1] == 0)
        {
            topo_sort(map->signposts[ways[i]->end - 1], map, visited_arr, order_arr, order_index);
        }
    }

    order_arr[*order_index] = current_signpost->key;
    (*order_index)++;
}

int *topo_order(map_t *map)
{
    int visited_arr[MAX_R] = {0};
    int order_arr[MAX_R] = {0};
    int order_index = 0;

    for (int i = 0; i < map->r; i++)
    {
        if (visited_arr[i] == 0)
        {
            topo_sort(map->signposts[i], map, visited_arr, order_arr, &order_index);
        }
    }

    int *result = malloc(sizeof(int) * map->r);
    if (result == NULL)
    {
        exit(1);
    }

    memcpy(result, order_arr, sizeof(int) * map->r);
    return result;
}

signpost_t *create_signpost(int key)
{
    signpost_t *s = malloc(sizeof(signpost_t));
    if (s == NULL)
    {
        exit(1);
    }

    s->key = key;
    s->num_of_ways = 0;
    s->ways_size = DEFAULT_WAYS_SIZE;

    s->ways = malloc(sizeof(way_t *) * s->ways_size);
    if (s->ways == NULL)
    {
        exit(1);
    }

    return s;
}

void add_way_to_signpost(signpost_t *s, way_t *w)
{
    if (s == NULL || w == NULL)
    {
        return;
    }

    if (s->num_of_ways >= s->ways_size)
    {
        s->ways_size *= 2;
        way_t **temp = realloc(s->ways, sizeof(way_t *) * s->ways_size);
        if (temp == NULL)
        {
            fprintf(stderr, "ERROR");
        }
        else
        {
            s->ways = temp;
        }
    }
    s->ways[s->num_of_ways] = w;
    s->num_of_ways++;
}

map_t *read_input()
{
    int r, t, p;
    scanf(" %d %d %d", &r, &t, &p);

    map_t *map = create_map(r, t, p);

    for (int i = 0; i < r; i++)
    {
        map->signposts[i] = create_signpost(i + 1);
    }

    for (int i = 0; i < t; i++)
    {
        int r1, r2, e, z;
        scanf(" %d %d %d %d", &r1, &r2, &e, &z);
        way_t *way = create_way(r1, r2, e, z);
        add_way_to_signpost(map->signposts[r1 - 1], way);
    }

    return map;
}

void dynamic_solution(map_t *map, int *order_arr)
{

    int back_cost_arr[MAX_R] = {0};
    int exp_arr[MAX_R][MAX_P + 1] = {{0}};

    for (int i = 0; i < map->r; i++)
    {
        for (int j = 0; j < map->p + 1; j++)
        {
            exp_arr[i][j] = -1;
        }
    }

    for (int i = 1; i < map->r; i++)
    {
        signpost_t *curr_post = map->signposts[order_arr[i] - 1];
        int back_cost = MAX_COST;

        for (int j = 0; j < curr_post->num_of_ways; j++)
        {
            int curr_back_cost = back_cost_arr[curr_post->ways[j]->end - 1] + curr_post->ways[j]->cost;
            if (curr_back_cost < back_cost)
            {
                back_cost = curr_back_cost;
            }

            int end_index = curr_post->ways[j]->end - 1;
            int front_cost = curr_post->ways[j]->cost;
            int front_exp = curr_post->ways[j]->exp;
            for (int k = 0; k < map->p + 1 - front_cost; k++)
            {
                if (exp_arr[end_index][k] == -1)
                {
                    continue;
                }
                if (exp_arr[end_index][k] + front_exp > exp_arr[curr_post->key - 1][front_cost + k])
                {
                    exp_arr[curr_post->key - 1][front_cost + k] = exp_arr[end_index][k] + front_exp;
                }
            }
        }

        for (int j = 0; j < map->p + 1; j++)
        {
            if (exp_arr[curr_post->key - 1][j] == 0)
            {
                exp_arr[curr_post->key - 1][j] = -1;
            }
        }

        back_cost_arr[curr_post->key - 1] = back_cost;
    }

    int big_maxi = -1;
    for (int i = 0; i < map->r; i++)
    {
        for (int j = 0; j < map->p + 1 - back_cost_arr[i]; j++)
        {
            if (exp_arr[i][j] > big_maxi)
            {
                big_maxi = exp_arr[i][j];
            }
        }
    }

    if (big_maxi == -1)
    {
        printf("-1\n");
    }
    else
    {
        printf("%d\n", big_maxi);
    }
}

int main(int argc, char const *argv[])
{
    map_t *m1 = read_input();

    int *order = topo_order(m1);

    dynamic_solution(m1, order);

    return 0;
}
