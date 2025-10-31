#include "list.h"

#include <stdio.h>


int main(void){
    list_t list1;
    list1.capacity = 10;

    listCtor(&list1);

    listInsertAfter(&list1, 0, 20);

    listInsertAfter(&list1, 0, 30);

    listInsertAfter(&list1, 0, 40);

    listInsertAfter(&list1, 0, 50);

    listInsertAfter(&list1, 0, 60);

    listDelete(&list1, 1);

    listInsertAfter(&list1, 3, 1000);

    listDtor(&list1);
}