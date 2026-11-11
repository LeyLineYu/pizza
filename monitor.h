#ifndef MONITOR_H
#define MONITOR_H

#include "constants.h"
#include <pthread.h>
#include <stdbool.h>

#define PIZZA_CHAR_COUNT 6 // 'pizza\0'
static const char* const EMPTY_PIZZA = "xxxxx";  
typedef struct {
    char contents[PIZZA_CHAR_COUNT];
} Pizza;

typedef struct {
	pthread_mutex_t m;

	Pizza plates[PLATE_COUNT];
	Pizza boxes [WAREHOUSE_CAPACITY];
	size_t warehouse_occupied; 
	size_t pizza_count; // cooked so far

	pthread_cond_t c_pizzamaker;
	pthread_cond_t c_courier;
} Monitor;

void monitor_init(Monitor* mon);
size_t monitor_get_pizza_count(Monitor* mon);
size_t monitor_get_warehouse_occupied(Monitor* mon);
bool monitor_is_pizza_ready(Monitor* mon, size_t i);
bool monitor_advance_pizza_stage(Monitor* mon, size_t plane_n, 
                                 const char* expected_stage, 
                                 const char* next_stage);
size_t monitor_push_pizza(Monitor* mon, size_t i);
Pizza monitor_pop_pizza(Monitor* mon);

#endif
