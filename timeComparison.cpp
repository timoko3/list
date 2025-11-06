#include "cashFriendlyList/list.h"
#include "classicalList/classicalList.h"

#include <time.h>

const size_t ITERATIONS_AMOUNT = 100000;

int main(void){
    list_t listCashFriendly;
    listCashFriendly.capacity = 3;

    listCtor(&listCashFriendly);

    listClassical_t listClassical;

    listClassicalCtor(&listClassical);


    clock_t start_time = clock();
    for(size_t iteration = 0; iteration < ITERATIONS_AMOUNT; iteration++){
        listInsertToTail(&listCashFriendly, 20);
        listDelete(&listCashFriendly, 1);
    }
    clock_t end_time = clock();
    double execution_time_cash_friendly = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("time cashFriendly: %lf\n", execution_time_cash_friendly);

    start_time = clock();
    for(size_t iteration = 0; iteration < ITERATIONS_AMOUNT; iteration++){
        curAnchorNode toDelete = listClassicalInsertToTail(&listClassical, 20);
        listClassicalDelete(&listClassical, toDelete);
    }
    end_time = clock();
    double execution_time_classical = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("time classical: %lf\n", execution_time_classical);

    printf("\nCashFriendlyList is %.2lf times faster than classicalList(amount of iterations %lu)\n", 
            execution_time_classical / execution_time_cash_friendly,
            ITERATIONS_AMOUNT );

    listDtor(&listCashFriendly);

    listClassicalDtor(&listClassical);
}