#include "pizzamaker.h"
#include "monitor.h"
#include <stdio.h>

static void pizzamaker_work(Monitor* mon, size_t plate_n);

void* pizzamaker_subroutine(void* data) {
    if (!data)
        return NULL;

    Monitor* mon = (Monitor*)data;
    size_t plate_n = 0;
    while (monitor_claim_plate(mon, &plate_n))
        pizzamaker_work(mon, plate_n);   // we own plate_n until released/pushed
                                    
    return NULL;
}

// Performs ONE step on a plate we own, then gives the plate back.
static void pizzamaker_work(Monitor* mon, size_t plate_n) {
    if (monitor_advance_pizza_stage(mon, plate_n, "xxxxx", "pxxxx"))
        printf("Тесто добавлено в пиццу #%zu\n", plate_n);
    else if (monitor_advance_pizza_stage(mon, plate_n, "pxxxx", "pixxx"))
        printf("Начинка добавлена в пиццу #%zu\n", plate_n);
    else if (monitor_advance_pizza_stage(mon, plate_n, "pixxx", "pizxx"))
        printf("Соус 1 добавлен в пиццу #%zu\n", plate_n);
    else if (monitor_advance_pizza_stage(mon, plate_n, "pizxx", "pizzx"))
        printf("Соус 2 добавлен в пиццу #%zu\n", plate_n);
    else if (monitor_advance_pizza_stage(mon, plate_n, "pizzx", "pizza"))
        printf("Выпекли пиццу #%zu\n", plate_n);
    else if (monitor_is_pizza_ready(mon, plate_n)) {
        // push releases the plate itself
        monitor_push_pizza(mon, plate_n);
        printf("Отнесли пиццу с тарелки #%zu\n", plate_n);
        return;
    }

    monitor_release_plate(mon, plate_n);
}
