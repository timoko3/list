#include "list.h"

#include <stdio.h>

int main(void){
    list_t list1;
    list1.capacity = 10;

    listCtor(&list1);

    listDumpBasic(&list1);

    listAdd(&list1, 10);
    listAdd(&list1, 20);
    listInsertAfter(&list1, 1, 17);
    listAdd(&list1, 40);
    listAdd(&list1, 50);

    listDumpBasic(&list1);

    listDelete(&list1, 3);

    listDumpBasic(&list1);

    listAdd(&list1, 69);

    listDumpBasic(&list1);

    listDtor(&list1);
}