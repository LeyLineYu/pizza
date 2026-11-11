#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "monitor.h"
#include "pizzamaker.h"
#include "courier.h"

void spawn_threads(Monitor* mon);

int main(){
	Monitor mon;
    monitor_init(&mon);

    spawn_threads(&mon);
    // monitor technically needs mutex and cond destructors, 
    // but man says they have virtually no effect
	return 0;
}

void spawn_threads(Monitor* mon) {
    if (!mon)
        return;

    pthread_attr_t attrs = {0};
    pthread_attr_init(&attrs);
    pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

    pthread_t pizzamakers[PIZZAMAKER_COUNT] = {0};
    for (size_t i = 0; i < PIZZAMAKER_COUNT; i++)
        pthread_create(&pizzamakers[i], &attrs, &pizzamaker_subroutine, &mon);

    pthread_t couriers[COURIER_COUNT] = {0};
    for (size_t i = 0; i < COURIER_COUNT; i++)
        pthread_create(&couriers[i], &attrs, &courier_subroutine, &mon);

    pthread_attr_destroy(&attrs); 
}
