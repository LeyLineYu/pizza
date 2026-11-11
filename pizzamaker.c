#include "pizzamaker.h"
#include "monitor.h"
#include <stdio.h>

static bool pizzamaker_find_plate(Monitor* mon, size_t* plate_n);
static void pizzamaker_work(Monitor* mon, size_t plate_n);

void* pizzamaker_subroutine(void* data) {
    if (!data)
        return NULL;

	Monitor* mon = (Monitor*)data;
	while (true) {
		size_t plate_n = 0;
	    if (pizzamaker_find_plate(mon, &plate_n))
            return NULL;

        pizzamaker_work(mon, plate_n);
	}
	return NULL;
}

static bool pizzamaker_find_plate(Monitor* mon, size_t* plate_n) {
    if (!mon || !plate_n)
        return true;

    while (true) {
        if (monitor_get_pizza_count(mon) >= PIZZA_COUNT)
            return true;

        for (size_t i = 0; i < PLATE_COUNT; i++) {
            if (!monitor_is_pizza_ready(mon, i) || 
                monitor_get_warehouse_occupied(mon) < WAREHOUSE_CAPACITY) {
                *plate_n = i;
                return false;
            }
        }
        pthread_cond_wait(&mon->c_pizzamaker, &mon->m);
    }

    // should be unreachable
    fprintf(stderr, "UNREACHABLE: pizzamaker_find_plate\n");
    return true;
}

static void pizzamaker_work(Monitor* mon, size_t plate_n) { 
    if (monitor_advance_pizza_stage(mon, plate_n, "xxxxx", "pxxxx")){
        printf("Тесто добавлено в пиццу #%zu\n", plate_n);
        return;
    }
    if (monitor_advance_pizza_stage(mon, plate_n, "pxxxx", "pixxx")){
        printf("Начинка добавлена в пиццу #%zu\n", plate_n);
        return;
    }
    if (monitor_advance_pizza_stage(mon, plate_n, "pixxx", "pizxx")){
        printf("Соус 1 добавлен в пиццу #%zu\n", plate_n);
        return;
    }
    if (monitor_advance_pizza_stage(mon, plate_n, "pizxx", "pizzx")){
        printf("Соус 2 добавлен в пиццу #%zu\n", plate_n);
        return;
    }
    if (monitor_advance_pizza_stage(mon, plate_n, "pizzx", "pizza")){
        printf("Выпекли пиццу #%zu\n", plate_n);
        return;
    }

    if (monitor_is_pizza_ready(mon, plate_n) == 0){
        size_t new_pizza_count = monitor_push_pizza(mon, plate_n);
        printf("Отнесли пиццу #%zu (в общем счёте это пицца #%zu)\n", 
                plate_n, new_pizza_count);

        pthread_cond_signal(&mon->c_courier);
    }
}
