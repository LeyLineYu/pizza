#include "courier.h"
#include "monitor.h"
#include <stdio.h>
#include <string.h>

void* courier_subroutine(void* data) {
    if (!data)
        return NULL;

    Monitor* mon = (Monitor*)data;
    Pizza pizza = {0};
    while (monitor_pop_pizza(mon, &pizza)) {
        if (strcmp(pizza.contents, "pizza") == 0)
            printf("Пицца правильная\n");
        else
            printf("Да твою-ж дивизию!\n");
    }

    return NULL;
}
