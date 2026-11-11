#include <string.h>
#include "monitor.h"

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

size_t monitor_push_pizza(Monitor* mon, size_t i) {
    pthread_mutex_lock(&mon->m);

    strncpy(mon->boxes[mon->warehouse_occupied].contents, 
            mon->plates[i].contents, PIZZA_CHAR_COUNT - 1);
    strcpy(mon->plates[i].contents, EMPTY_PIZZA);
    mon->warehouse_occupied++;
    mon->pizza_count++;
    size_t new_pizza_count = mon->pizza_count;

    pthread_mutex_unlock(&mon->m);

    return new_pizza_count;
}

Pizza monitor_pop_pizza(Monitor* mon) {
    pthread_mutex_lock(&mon->m);

    mon->warehouse_occupied--;
	Pizza pizza = {0};
	strncpy(pizza.contents, 
            mon->boxes[mon->warehouse_occupied].contents,
            PIZZA_CHAR_COUNT - 1);

    pthread_mutex_unlock(&mon->m);
    return pizza;
}

size_t monitor_get_pizza_count(Monitor* mon) {
	pthread_mutex_lock(&mon->m);
    size_t pizza_count = mon->pizza_count;
    pthread_mutex_unlock(&mon->m);
    return pizza_count;
}

size_t monitor_get_warehouse_occupied(Monitor* mon) {
	pthread_mutex_lock(&mon->m);
    size_t warehouse_occupied = mon->warehouse_occupied;
    pthread_mutex_unlock(&mon->m);
    return warehouse_occupied;
}

bool monitor_is_pizza_ready(Monitor* mon, size_t i) {
	pthread_mutex_lock(&mon->m);
    int res = memcmp(mon->plates[i].contents, "pizza", PIZZA_CHAR_COUNT - 1);
    pthread_mutex_unlock(&mon->m);
    return res == 0 ? true : false;
}

bool monitor_advance_pizza_stage(Monitor* mon, size_t i, 
                                 const char* expected_stage, 
                                 const char* next_stage) {
	pthread_mutex_lock(&mon->m);
    bool success = false;
    if (memcmp(mon->plates[i].contents, 
               expected_stage, PIZZA_CHAR_COUNT - 1) == 0) {
        strncpy(mon->plates[i].contents, 
                next_stage, PIZZA_CHAR_COUNT - 1);
        success = true;
    }
    pthread_mutex_unlock(&mon->m);
    return success;
}
