#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

enum place {
    NUZKY, VRTACKA, OHYBACKA, SVARECKA, LAKOVNA, SROUBOVAK, FREZA,
    _PLACE_COUNT
};

const char *place_str[_PLACE_COUNT] = {
    [NUZKY] = "nuzky",
    [VRTACKA] = "vrtacka",
    [OHYBACKA] = "ohybacka",
    [SVARECKA] = "svarecka",
    [LAKOVNA] = "lakovna",
    [SROUBOVAK] = "sroubovak",
    [FREZA] = "freza",
};

enum product {
    A, B, C,
    _PRODUCT_COUNT
};

const char *product_str[_PRODUCT_COUNT] = {
    [A] = "A",
    [B] = "B",
    [C] = "C",
};

const int time_per_place[_PLACE_COUNT] = {
        [NUZKY] = 100000,
        [VRTACKA] = 200000,
        [OHYBACKA] = 150000,
        [SVARECKA] = 300000,
        [LAKOVNA] = 400000,
        [SROUBOVAK] = 250000,
        [FREZA] = 500000,
};

int find_string_in_array(const char **array, int length, char *what)
{
    for (int i = 0; i < length; i++)
        if (strcmp(array[i], what) == 0)
            return i;
    return -1;
}

int ready_places[_PLACE_COUNT] = {0, 0, 0, 0, 0, 0, 0};
int total_places[_PLACE_COUNT] = {0, 0, 0, 0, 0, 0, 0};
int remove_places[_PLACE_COUNT] = {0, 0, 0, 0, 0, 0, 0};

#define _PHASE_COUNT 6
int parts[_PRODUCT_COUNT][_PHASE_COUNT] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0}};

const int part_places[_PRODUCT_COUNT][_PHASE_COUNT] = {
        {NUZKY, VRTACKA, OHYBACKA, SVARECKA, VRTACKA, LAKOVNA},
        {VRTACKA, NUZKY, FREZA, VRTACKA, LAKOVNA, SROUBOVAK},
        {FREZA, VRTACKA, SROUBOVAK, VRTACKA, FREZA, LAKOVNA}
};

struct Worker {
    char* name;
    int workplace;
    bool end;
};

struct Worker** workers;

int worker_not_working = 0;

bool input_done = false;

pthread_mutex_t mutex_worker;
pthread_mutex_t mutex_place;
pthread_mutex_t mutex_part;
pthread_mutex_t mutex_printf;
pthread_mutex_t mutex_worker_not_working;
pthread_mutex_t mutex_input;
pthread_mutex_t mutex_wait_worker;
pthread_mutex_t mutex_wait_main;

pthread_cond_t cond_worker;
pthread_cond_t cond_main;

bool is_work_done(const int workers_active) {
    pthread_mutex_lock(&mutex_worker_not_working);
    pthread_mutex_lock(&mutex_printf);
    fprintf(stderr, "workers_active: %d, worker_not_working: %d\n", workers_active, worker_not_working);
    pthread_mutex_unlock(&mutex_printf);
    if (workers_active == worker_not_working) {
        pthread_mutex_unlock(&mutex_worker_not_working);
        return true;
    }
    pthread_mutex_unlock(&mutex_worker_not_working);
    return false;
}

bool is_ready(int* step, int* product, const int workplace) {
    pthread_mutex_lock(&mutex_place);
    if (ready_places[workplace] == 0) {
        pthread_mutex_unlock(&mutex_place);

        return false;
    }

    pthread_mutex_lock(&mutex_part);
    for (int j = _PHASE_COUNT - 1; j >= 0; j--) {
        for (int i = 0; i < _PRODUCT_COUNT; i++) {
            if (workplace == part_places[i][j]) {
                /*pthread_mutex_lock(&mutex_printf);
                fprintf(stderr, "Workplace: %d, i: %d, j: %d, parts: %d\n", workplace, i, j, parts[i][j]);
                pthread_mutex_unlock(&mutex_printf);*/
                if (parts[i][j] > 0) {
                    ready_places[workplace]--;
                    parts[i][j]--;
                    (*product) = i;
                    (*step) = j;

                    pthread_mutex_lock(&mutex_worker_not_working);
                    //worker_not_working = 0;
                    pthread_mutex_unlock(&mutex_worker_not_working);

                    pthread_mutex_unlock(&mutex_part);
                    pthread_mutex_unlock(&mutex_place);

                    return true;
                }
            }
        }
    }
    pthread_mutex_unlock(&mutex_part);
    pthread_mutex_unlock(&mutex_place);

    return false;
}

void* work(void* id) {
    int index = *(int *)(id);
    pthread_mutex_lock(&mutex_worker);
    struct Worker* ptr = workers[index];
    pthread_mutex_unlock(&mutex_worker);

    int step = 0, product = 0;

    while (true) {
        pthread_mutex_lock(&mutex_wait_worker);
        pthread_mutex_lock(&mutex_worker);
        while (!ptr->end && !is_ready(&step, &product, ptr->workplace)) {
            pthread_mutex_unlock(&mutex_worker);

            pthread_mutex_lock(&mutex_wait_main);
            pthread_mutex_lock(&mutex_input);
            if (input_done) {
                pthread_mutex_lock(&mutex_printf);
                fprintf(stderr, "thread id: %d\n", index);
                pthread_mutex_unlock(&mutex_printf);
                pthread_cond_broadcast(&cond_main);
            }
            pthread_mutex_unlock(&mutex_input);
            pthread_mutex_unlock(&mutex_wait_main);

            pthread_cond_wait(&cond_worker, &mutex_wait_worker);

            pthread_mutex_lock(&mutex_worker);
        }
        pthread_mutex_unlock(&mutex_worker);
        pthread_mutex_unlock(&mutex_wait_worker);

        pthread_mutex_lock(&mutex_worker);
        if (ptr->end == true) {
            free(ptr->name);
            free(ptr);
            workers[index] = NULL;
            pthread_mutex_unlock(&mutex_worker);
            return NULL;
        }
        pthread_mutex_unlock(&mutex_worker);

        // TODO: possible missing mutex
        pthread_mutex_lock(&mutex_printf);
        printf("%s %s %d %s\n", ptr->name, place_str[ptr->workplace], step + 1, product_str[product]);
        pthread_mutex_unlock(&mutex_printf);

        usleep(time_per_place[ptr->workplace]);

        pthread_mutex_lock(&mutex_place);
        if (remove_places[ptr->workplace] == 0) {
            ready_places[ptr->workplace]++;
        } else {
            remove_places[ptr->workplace]--;
        }
        pthread_mutex_unlock(&mutex_place);

        if (step == 5) {
            pthread_mutex_lock(&mutex_printf);
            printf("done %s\n", product_str[product]);
            pthread_mutex_unlock(&mutex_printf);
        } else {
            pthread_mutex_lock(&mutex_part);
            /*pthread_mutex_lock(&mutex_printf);
            fprintf(stderr, "i: %d, j: %d, parts: %d\n", product, step + 1, parts[product][step + 1]);*/
            parts[product][step + 1]++;
            /*fprintf(stderr, "parts: %d\n", parts[product][step + 1]);
            pthread_mutex_unlock(&mutex_printf);*/
            pthread_mutex_unlock(&mutex_part);
        }

        pthread_mutex_lock(&mutex_worker_not_working);
        worker_not_working = 0;
        pthread_mutex_unlock(&mutex_worker_not_working);

        pthread_mutex_lock(&mutex_wait_worker);
        pthread_cond_broadcast(&cond_worker);
        pthread_mutex_unlock(&mutex_wait_worker);
    }
}

int main(int argc, char **argv)
{
    pthread_mutex_init(&mutex_worker, NULL);
    pthread_mutex_init(&mutex_place, NULL);
    pthread_mutex_init(&mutex_part, NULL);
    pthread_mutex_init(&mutex_printf, NULL);
    pthread_mutex_init(&mutex_worker_not_working, NULL);
    pthread_mutex_init(&mutex_input, NULL);
    pthread_mutex_init(&mutex_wait_worker, NULL);
    pthread_mutex_init(&mutex_wait_main, NULL);

    pthread_cond_init(&cond_worker, NULL);
    pthread_cond_init(&cond_main, NULL);

    int workers_size = 0, workers_capacity = 20, workers_active = 0;

    workers = (struct Worker **) malloc(workers_capacity * sizeof(struct Worker*));

    pthread_t* threads = (pthread_t *) malloc((workers_capacity) * sizeof(pthread_t));
    int* worker_id = (int *) malloc((workers_capacity) * sizeof(int));

    /*
    for (int i = 0; i < _PLACE_COUNT; i++) {
        ready_places[i] = 0;
        total_places[i] = 0;
    }

    for (int i = 0; i < _PRODUCT_COUNT; i++) {
        for (int j = 0; j < _PHASE_COUNT; j++) {
            parts[i][j] = 0;
        }
    }
    */

    while (1) {
        char *line, *cmd, *arg1, *arg2, *arg3, *saveptr;
        int s = scanf(" %m[^\n]", &line);
        if (s == EOF) {
            break;
        }
        if (s == 0) {
            continue;
        }

        cmd  = strtok_r(line, " ", &saveptr);
        arg1 = strtok_r(NULL, " ", &saveptr);
        arg2 = strtok_r(NULL, " ", &saveptr);
        arg3 = strtok_r(NULL, " ", &saveptr);

        if (strcmp(cmd, "start") == 0 && arg1 && arg2 && !arg3) {
            if (strlen(arg1) > 30) {
                free(line);
                continue;
            }

            int workplace = find_string_in_array(place_str, _PLACE_COUNT, arg2);
            char *name = strdup(arg1);

            if (workplace >= 0 && name != NULL) {
                workers_size++;
                workers_active++;
                if (workers_size == workers_capacity) {
                    workers_capacity <<= 1;
                    threads = (pthread_t *) realloc(threads, workers_capacity * sizeof(pthread_t));
                    worker_id = (int *) realloc(worker_id, workers_capacity * sizeof(int));
                    workers = (struct Worker **) realloc(workers, workers_capacity * sizeof(struct Worker *));
                }
                worker_id[workers_size - 1] = workers_size - 1;
                workers[workers_size - 1] = (struct Worker *) malloc(sizeof(struct Worker));
                workers[workers_size - 1]->name = name;
                workers[workers_size - 1]->workplace = find_string_in_array(place_str, _PLACE_COUNT, arg2);
                workers[workers_size - 1]->end = false;

                pthread_create(&threads[workers_size - 1], NULL, work, (void *)(&worker_id[workers_size - 1]));
            } else {
                free(name);
            }
        } else if (strcmp(cmd, "make") == 0 && arg1 && !arg2) {
            int product = find_string_in_array(product_str, _PRODUCT_COUNT, arg1);

            if (product >= 0) {
                pthread_mutex_lock(&mutex_part);
                parts[product][0]++;
                pthread_mutex_unlock(&mutex_part);

                pthread_mutex_lock(&mutex_wait_worker);
                pthread_cond_broadcast(&cond_worker);
                pthread_mutex_unlock(&mutex_wait_worker);
            }
        } else if (strcmp(cmd, "end") == 0 && arg1 && !arg2) {
            workers_active--;

            pthread_mutex_lock(&mutex_worker);
            for (int i = 0; i < workers_size; i++) {
                if (strcmp(workers[i]->name, arg1) == 0) {
                    workers[i]->end = true;
                }
            }
            pthread_mutex_unlock(&mutex_worker);

            pthread_mutex_lock(&mutex_wait_worker);
            pthread_cond_broadcast(&cond_worker);
            pthread_mutex_unlock(&mutex_wait_worker);

        } else if (strcmp(cmd, "add") == 0 && arg1 && !arg2) {
            int place = find_string_in_array(place_str, _PLACE_COUNT, arg1);

            if (place >= 0) {
                pthread_mutex_lock(&mutex_place);
                ready_places[place]++;
                total_places[place]++;
                pthread_mutex_unlock(&mutex_place);

                pthread_mutex_lock(&mutex_wait_worker);
                pthread_cond_broadcast(&cond_worker);
                pthread_mutex_unlock(&mutex_wait_worker);
            }
        } else if (strcmp(cmd, "remove") == 0 && arg1 && !arg2) {
            int place = find_string_in_array(place_str, _PLACE_COUNT, arg1);

            if (place >= 0) {
                pthread_mutex_lock(&mutex_place);
                if (ready_places[place] > 0 && total_places[place] > 0) {
                    ready_places[place]--;
                    total_places[place]--;
                } else if (total_places[place] > 0) {
                    remove_places[place]++;
                    total_places[place]--;
                }
                pthread_mutex_unlock(&mutex_place);
            }
        } else {
            fprintf(stderr, "Invalid command: %s\n", line);
        }
        free(line);
    }

    pthread_mutex_lock(&mutex_input);
    input_done = true;
    pthread_mutex_unlock(&mutex_input);

    pthread_mutex_lock(&mutex_wait_main);
    while (!is_work_done(workers_active)) {
        pthread_cond_wait(&cond_main, &mutex_wait_main);

        pthread_mutex_lock(&mutex_worker_not_working);
        worker_not_working++;
        pthread_mutex_unlock(&mutex_worker_not_working);
    }
    pthread_mutex_unlock(&mutex_wait_main);

    pthread_mutex_lock(&mutex_worker);
    for (int i = 0; i < workers_size; i++) {
        if (workers[i] != NULL) {
            workers[i]->end = true;
        }
    }
    pthread_mutex_unlock(&mutex_worker);

    pthread_mutex_lock(&mutex_printf);
    fprintf(stderr, "END\n");
    pthread_mutex_unlock(&mutex_printf);

    pthread_mutex_lock(&mutex_wait_worker);
    pthread_cond_broadcast(&cond_worker);
    pthread_mutex_unlock(&mutex_wait_worker);

    for (int i = 0; i < workers_size; i++) {
        pthread_join(threads[i], NULL);
    }

    free(worker_id);
    free(threads);
    free(workers);

    pthread_mutex_destroy(&mutex_worker);
    pthread_mutex_destroy(&mutex_place);
    pthread_mutex_destroy(&mutex_part);
    pthread_mutex_destroy(&mutex_printf);
    pthread_mutex_destroy(&mutex_worker_not_working);
    pthread_mutex_destroy(&mutex_input);
    pthread_mutex_destroy(&mutex_wait_worker);
    pthread_mutex_destroy(&mutex_wait_main);

    pthread_cond_destroy(&cond_worker);
    pthread_cond_destroy(&cond_main);

    return 0;
}
