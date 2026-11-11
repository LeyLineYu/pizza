#include <stdio.h>
#include <string.h>
#include "monitor.h"

// Caller must hold mon->m
static bool plate_is_ready(const Pizza* p) {
    return strcmp(p->contents, "pizza") == 0;
}

void monitor_init(Monitor* mon) {
    if (!mon)
        return;

    *mon = (Monitor){0};
    pthread_mutex_init(&mon->m, NULL);
    pthread_cond_init(&mon->c_pizzamaker, NULL);
    pthread_cond_init(&mon->c_courier, NULL);
    for (size_t i = 0; i < PLATE_COUNT; i++)
        strcpy(mon->plates[i].contents, EMPTY_PIZZA);
}

// Waits until there is a plate this maker may work on, and claims it.
// Returns false when all pizzas are done.
bool monitor_claim_plate(Monitor* mon, size_t* plate_n) {
    pthread_mutex_lock(&mon->m);
    while (true) {
        if (mon->pizza_count >= PIZZA_COUNT) {
            pthread_mutex_unlock(&mon->m);
            return false;
        }

        for (size_t i = 0; i < PLATE_COUNT; i++) {
            if (mon->busy[i])
                continue;

            bool empty = strcmp(mon->plates[i].contents, EMPTY_PIZZA) == 0;
            bool ready = plate_is_ready(&mon->plates[i]);

            if (empty && mon->started >= PIZZA_COUNT)
                continue;
            if (ready && mon->warehouse_occupied >= WAREHOUSE_CAPACITY)
                continue;

            if (empty)
                mon->started++;
            mon->busy[i] = true;
            *plate_n = i;
            pthread_mutex_unlock(&mon->m);
            return true;
        }

        pthread_cond_wait(&mon->c_pizzamaker, &mon->m);
    }
}

void monitor_release_plate(Monitor* mon, size_t plate_n) {
    pthread_mutex_lock(&mon->m);
    mon->busy[plate_n] = false;
    pthread_cond_broadcast(&mon->c_pizzamaker);
    pthread_mutex_unlock(&mon->m);
}

bool monitor_is_pizza_ready(Monitor* mon, size_t plate_n) {
    pthread_mutex_lock(&mon->m);
    bool res = plate_is_ready(&mon->plates[plate_n]);
    pthread_mutex_unlock(&mon->m);
    return res;
}

bool monitor_advance_pizza_stage(Monitor* mon, size_t plate_n,
                                 const char* expected_stage,
                                 const char* next_stage) {
    pthread_mutex_lock(&mon->m);
    bool success = false;
    if (strcmp(mon->plates[plate_n].contents, expected_stage) == 0) {
        strcpy(mon->plates[plate_n].contents, next_stage);
        success = true;
    }
    pthread_mutex_unlock(&mon->m);
    return success;
}

void monitor_push_pizza(Monitor* mon, size_t plate_n) {
    pthread_mutex_lock(&mon->m);
    while (mon->warehouse_occupied == WAREHOUSE_CAPACITY)
        pthread_cond_wait(&mon->c_pizzamaker, &mon->m);

    mon->boxes[mon->warehouse_occupied] = mon->plates[plate_n];
    strcpy(mon->plates[plate_n].contents, EMPTY_PIZZA);
    mon->warehouse_occupied++;
    mon->busy[plate_n] = false;                 // plate released

    pthread_cond_broadcast(&mon->c_courier);    // new pizza / maybe finished
    pthread_cond_broadcast(&mon->c_pizzamaker); // free plate / maybe finished
    pthread_mutex_unlock(&mon->m);
}

// Returns false when there is nothing left to deliver.
bool monitor_pop_pizza(Monitor* mon, Pizza* out) {
    pthread_mutex_lock(&mon->m);
    while (mon->warehouse_occupied == 0) {
        if (mon->pizza_count >= PIZZA_COUNT) {
            pthread_mutex_unlock(&mon->m);
            return false;
        }
        pthread_cond_wait(&mon->c_courier, &mon->m);
    }
    mon->warehouse_occupied--;
    mon->pizza_count++;
    *out = mon->boxes[mon->warehouse_occupied];
    pthread_cond_broadcast(&mon->c_pizzamaker); // space freed
    pthread_mutex_unlock(&mon->m);
    return true;
}
