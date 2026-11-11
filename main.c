#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "monitor.h"
#include "pizzamaker.h"
#include "courier.h"

void execute_threads(Monitor* mon);

int main(){
	Monitor mon;
    monitor_init(&mon);

    execute_threads(&mon);
    // monitor technically needs mutex and cond destructors, 
    // but man says they have virtually no effect
	return 0;
}

void execute_threads(Monitor* mon) {
    if (!mon)
        return;

    pthread_t pizzamakers[PIZZAMAKER_COUNT] = {0};
    for (size_t i = 0; i < PIZZAMAKER_COUNT; i++)
        pthread_create(&pizzamakers[i], NULL, &pizzamaker_subroutine, &mon);

    pthread_t couriers[COURIER_COUNT] = {0};
    for (size_t i = 0; i < COURIER_COUNT; i++)
        pthread_create(&couriers[i], NULL, &courier_subroutine, &mon);

    for (size_t i = 0; i < PIZZAMAKER_COUNT; i++)
        pthread_join(pizzamakers[i], NULL);
    for (size_t i = 0; i < COURIER_COUNT; i++)
        pthread_join(couriers[i], NULL);
}
