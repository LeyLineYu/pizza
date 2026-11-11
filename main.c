#include <stdio.h>
#include <pthread.h>

void* subroutine(void* data);

#define COUNT     10000000
#define N_THREADS 10

typedef struct {
    pthread_mutex_t m;
    unsigned count;
    pthread_cond_t c1;
} Monitor;

void monitor_increase_count(Monitor* mon);

int main(void) {
    printf("Hello, World\n");

    Monitor mon = {0};
    pthread_mutex_init(&mon.m, NULL);
    pthread_cond_init(&mon.c1, NULL);

    pthread_t threads[N_THREADS] = {0};
    for (size_t i = 0; i < N_THREADS; i++) {
        pthread_create(&threads[i], NULL, &subroutine, &mon);
    }
    for (size_t i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Total:    %d\n"
           "Expected: %d\n",
           mon.count,
           N_THREADS * COUNT);
    return 0;
}

void* subroutine(void* data) {
    Monitor* mon = (Monitor*)data;
    for (size_t i = 0; i < COUNT; i++) {
        monitor_increase_count(mon);
    }
    return NULL;
}

void monitor_increase_count(Monitor* mon) {
        pthread_mutex_lock(&mon->m);
        (mon->count)++;
        pthread_mutex_unlock(&mon->m);
}
