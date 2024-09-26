#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_WAYS_SIZE 10

#define MAXI 100000000

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
    {
        exit(1);
    }

    w->start = start;
    w->end = end;
    w->exp = exp;
    w->cost = cost;

    return w;
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

map_t *create_map(int r, int t, int p)
{
    map_t *m = malloc(sizeof(map_t));
    if (m == NULL)
    {
        exit(1);
    }

    m->r = r;
    m->t = t;
    m->p = p;

    m->signposts = malloc(sizeof(signpost_t *) * r);
    if (m->signposts == NULL)
    {
        exit(1);
    }

    return m;
}

void add_way_to_signpost(signpost_t *s, way_t *w)
{
    if (s->num_of_ways >= s->ways_size)
    {
        s->ways_size *= 2;
        s->ways = realloc(s->ways, sizeof(way_t *) * s->ways_size);
        if (s->ways == NULL)
        {
            exit(1);
        }
    }
    s->ways[s->num_of_ways] = w;
    s->num_of_ways++;
}

void topo_sort(signpost_t *signpost, map_t *map, int *visited_arr, int *order_arr, int *order_index)
{
    visited_arr[signpost->key - 1] = 1;
    for (size_t i = 0; i < signpost->num_of_ways; i++)
    {
        if (visited_arr[signpost->ways[i]->end - 1] == 0)
        {
            topo_sort(map->signposts[signpost->ways[i]->end - 1], map, visited_arr, order_arr, order_index);
        }
    }
    order_arr[*order_index] = signpost->key;
    (*order_index)++;
}

int *topo_order(map_t *map)
{
    int *visited_arr = calloc(map->r, sizeof(int));
    int *order_arr = malloc(sizeof(int) * map->r);
    int *order_index = calloc(1, sizeof(int));

    for (size_t i = 0; i < map->r; i++)
    {
        if (visited_arr[i] == 0)
        {
            topo_sort(map->signposts[i], map, visited_arr, order_arr, order_index);
        }
    }

    /*
    printf("topological order:\n");
    for (size_t i = 0; i < map->r; i++)
    {
        printf("\t%d\n", order_arr[i]);
    }
    */

    return order_arr;
}

map_t *read_input()
{
    int r, t, p;
    scanf(" %d %d %d", &r, &t, &p);

    map_t *map = create_map(r, t, p);

    for (size_t i = 0; i < r; i++)
    {
        map->signposts[i] = create_signpost(i + 1);
    }

    way_t **all_ways = malloc(sizeof(way_t *) * t);
    if (all_ways == NULL)
    {
        exit(1);
    }

    int r1, r2, e, z;
    for (size_t i = 0; i < t; i++)
    {
        scanf(" %d %d %d %d", &r1, &r2, &e, &z);
        way_t *way = create_way(r1, r2, e, z);
        all_ways[i] = way;

        add_way_to_signpost(map->signposts[r1 - 1], way);
    }

    return map;
}

void print_map(map_t *map)
{
    printf("map: r %d t %d p %d\n", map->r, map->t, map->p);
    for (size_t i = 0; i < map->r; i++)
    {
        printf("signpost %d:\n", map->signposts[i]->key);
        for (size_t j = 0; j < map->signposts[i]->num_of_ways; j++)
        {
            way_t *way = map->signposts[i]->ways[j];
            printf("\tway: %d %d %d %d\n", way->start, way->end, way->exp, way->cost);
        }
    }
}

void dynamic_solution(map_t *map, int *order_arr)
{

    int *back_cost_arr = calloc(map->r, sizeof(int));
    int *exp_arr = calloc(map->r * (map->p + 1), sizeof(int));
    //int *exp_track_arr = calloc(map->r * (map->p + 1), sizeof(int));
    //int *exp_connect_exp_arr = calloc(map->r * (map->p + 1), sizeof(int));
    //int *exp_connect_cost_arr = calloc(map->r * (map->p + 1), sizeof(int));
    for (size_t i = 1; i < map->p + 1; i++)
    {
        exp_arr[i] = -1;
    }

    for (int i = 1; i < map->r; i++)
    {
        signpost_t *curr_post = map->signposts[order_arr[i] - 1];
        int back_cost = MAXI;

        //printf("post %d:\n", curr_post->key);

        int curr_index = (curr_post->key - 1) * (map->p + 1);

        for (int j = 0; j < curr_post->num_of_ways; j++)
        {
            int curr_back_cost = back_cost_arr[curr_post->ways[j]->end - 1] + curr_post->ways[j]->cost;
            if (curr_back_cost < back_cost)
            {
                back_cost = curr_back_cost;
            }

            int end_index = (curr_post->ways[j]->end - 1) * (map->p + 1);
            int front_cost = curr_post->ways[j]->cost;
            int front_exp = curr_post->ways[j]->exp;
            
            for (int k = 0; k < map->p + 1 - front_cost; k++)
            {
                if (exp_arr[end_index + k] == -1)
                {
                    //exp_arr[curr_index + front_cost + k] = -1;
                    continue;
                }

                if (exp_arr[end_index + k] + front_exp > exp_arr[curr_index + front_cost + k])
                {
                    exp_arr[curr_index + front_cost + k] = exp_arr[end_index + k] + front_exp;
                }
            }
        }

        for (int j = 0; j < map->p + 1; j++)
        {
            if (exp_arr[curr_index + j] == 0)
            {
                exp_arr[curr_index + j] = -1;
            }
        }

        /*
        for (size_t k = 0; k < map->p + 1; k++)
        {
            printf("\t glob cost: %ld glob exp: %d track: %d ", k, exp_arr[(curr_post->key - 1) * (map->p + 1) + k], exp_track_arr[(curr_post->key - 1) * (map->p + 1) + k]);
            printf("connect cost: %d connect exp: %d\n", exp_connect_cost_arr[(curr_post->key - 1) * (map->p + 1) + k], exp_connect_exp_arr[(curr_post->key - 1) * (map->p + 1) + k]);
        }
        */
        
        
        
        back_cost_arr[curr_post->key - 1] = back_cost;

        //printf("\tback cost %d\n", back_cost);
    }

    int big_maxi = -1;
    for (size_t i = 0; i < map->r; i++)
    {
        // printf("key %ld:\n", i+1);
        for (size_t j = 0; j < map->p + 1 - back_cost_arr[i]; j++)
        {
            // printf("\t%d\n",exp_arr[i*(map->p + 1) + j]);
            if (exp_arr[i * (map->p + 1) + j] > big_maxi)
            {
                big_maxi = exp_arr[i * (map->p + 1) + j];
            }
        }
    }
    printf("%d\n", big_maxi);
}

int main(int argc, char const *argv[])
{
    map_t *m1 = read_input();

    // print_map(m1);

    int *order = topo_order(m1);

    dynamic_solution(m1, order);

    return 0;
}
