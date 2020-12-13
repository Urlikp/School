#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t mutex_queue;
pthread_mutex_t mutex_printf;

sem_t semaphore;

int thread_count;

struct Command {
    int x;
    char* text;
    struct Command* next;
};

struct Command* head;
struct Command* tail;

/*struct Queue {
    struct Command* head;
    struct Command* tail;
    int thread_count;
};*/

void* consument(void* id) {
    //struct Queue* queue = q;
    int thread_id = *(int*)(id);

    while (1) {
        sem_wait(&semaphore);

        pthread_mutex_lock(&mutex_queue);
        if (/*queue->*/head != NULL) {
            struct Command *popped_command = /*queue->*/head;
            /*queue->*/head = /*queue->*/head->next;
            pthread_mutex_unlock(&mutex_queue);

            pthread_mutex_lock(&mutex_printf);
            printf("Thread %d:", thread_id);
            for (size_t i = 0; i < popped_command->x; i++) {
                printf(" %s", popped_command->text);
            }
            printf("\n");

            free(popped_command->text);
            free(popped_command);
            pthread_mutex_unlock(&mutex_printf);
        } else {
            pthread_mutex_unlock(&mutex_queue);
            return NULL;
        }
    }
}

void* producent() {
    int ret, x;
    char *text;
    //struct Queue* queue = q;
    while ((ret = scanf("%d %ms", &x, &text)) == 2) {
        if (x < 0) {
            ret = 1;
            break;
        }

        struct Command* new_command = (struct Command*) malloc(sizeof(struct Command));
        new_command->x = x;
        new_command->text = text;
        new_command->next = NULL;

        pthread_mutex_lock(&mutex_queue);
        if (/*queue->*/head == NULL) {
            /*queue->*/head = new_command;
            /*queue->*/tail = new_command;
        } else {
            /*queue->*/tail->next = new_command;
            /*queue->*/tail = new_command;
        }
        text = NULL;
        pthread_mutex_unlock(&mutex_queue);

        sem_post(&semaphore);
    }

    pthread_mutex_lock(&mutex_queue);
    for (size_t i = 0; i < /*queue->*/thread_count-1; i++) {
        sem_post(&semaphore);
    }
    pthread_mutex_unlock(&mutex_queue);

    if (ret == EOF) {
        return NULL;
    } else {
        return (void*)1;
    }
}

int main(int argc, char *argv[]) {
    thread_count = 2;
    if (argc > 1) {
        char* p;
        thread_count = strtol(argv[1], &p, 10) + 1;
        if (*p != '\0' || errno != 0) {
            return 1;
        }
    }

    if (thread_count - 1 > sysconf(_SC_NPROCESSORS_ONLN)) {
        return 1;
    }

    //struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    /*queue->*/head = NULL;
    /*queue->*/tail = NULL;
    /*queue->thread_count = thread_count;*/

    pthread_t* thread = malloc((thread_count) * sizeof(pthread_t));
    int** producent_ret = malloc(sizeof(int*));
    int* thread_id = malloc((thread_count - 1) * sizeof(int));

    pthread_mutex_init(&mutex_queue, NULL);
    pthread_mutex_init(&mutex_printf, NULL);
    sem_init(&semaphore, 0, 0);

    pthread_create(&thread[0], NULL, producent, NULL/*(void*)queue*/);

    for (size_t i = 1; i < thread_count; i++) {
        thread_id[i - 1] = i;
        pthread_create(&thread[i], NULL, consument, (void*)(&thread_id[i - 1])/*(void*)queue*/);
    }

    pthread_join(thread[0], (void**)producent_ret);

    for (size_t i = 1; i < thread_count; i++) {
        pthread_join(thread[i], NULL);
    }

    int ret = *(int*)(producent_ret);

    pthread_mutex_destroy(&mutex_queue);
    pthread_mutex_destroy(&mutex_printf);
    sem_destroy(&semaphore);

    //free(queue);
    free(thread);
    free(producent_ret);
    free(thread_id);

    return ret;
}
