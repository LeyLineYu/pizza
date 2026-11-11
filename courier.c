#include "courier.h"
#include "monitor.h"
#include <stdio.h>
#include <string.h>

void* courier_subroutine(void* data) {
    if (!data)
        return NULL;

	Monitor* mon = (Monitor*) data;
	while (true){
		while (monitor_get_warehouse_occupied(mon) == 0)
			pthread_cond_wait(&mon->c_courier, &mon->m);

        Pizza pizza = monitor_pop_pizza(mon);

		if (memcmp(pizza.contents, "pizza", PIZZA_CHAR_COUNT) == 0)
			printf("Пицца правильная\n");
        else
            printf("Да твою-ж дивизию!\n");

		pthread_cond_signal(&mon->c_pizzamaker);
	}
	return NULL;
}
