#ifndef MONITOR_H
#define MONITOR_H

#include "constants.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

#define PIZZA_CHAR_COUNT 6 // 'pizza\0'
static const char* const EMPTY_PIZZA = "xxxxx";

typedef struct {
    char contents[PIZZA_CHAR_COUNT];
} Pizza;

typedef struct {
    pthread_mutex_t m;

    Pizza  plates[PLATE_COUNT];
    bool   busy[PLATE_COUNT];          // plate is owned by some pizzamaker
    Pizza  boxes[WAREHOUSE_CAPACITY];
    size_t warehouse_occupied;
    size_t started;                    // pizzas started so far (<= PIZZA_COUNT)
    size_t pizza_count;                // pizzas finished and put in warehouse

    pthread_cond_t c_pizzamaker;
    pthread_cond_t c_courier;
} Monitor;

void   monitor_init(Monitor* mon);

// Pizzamaker side
bool   monitor_claim_plate(Monitor* mon, size_t* plate_n); // false => maker should exit
void   monitor_release_plate(Monitor* mon, size_t plate_n);
bool   monitor_is_pizza_ready(Monitor* mon, size_t plate_n);
bool   monitor_advance_pizza_stage(Monitor* mon, size_t plate_n,
                                   const char* expected_stage,
                                   const char* next_stage);
void   monitor_push_pizza(Monitor* mon, size_t plate_n);   // blocks while warehouse is full

// Courier side
bool   monitor_pop_pizza(Monitor* mon, Pizza* out);        // false => courier should exit

#endif
