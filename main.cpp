#include "list.h"

#include <stdio.h>

int main(void){
    list_t list1;
    list1.capacity = 10;

    listCtor(&list1);

    listDumpBasic(&list1);

    listAdd(&list1, 10);
    listAdd(&list1, 3);
    listAdd(&list1, 1);

    listDumpBasic(&list1);

    listDtor(&list1);
}