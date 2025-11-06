#include "list.h"

#include <stdio.h>


int main(void){
    list_t list1;
    list1.capacity = 7;

    listCtor(&list1);

    listInsertToTail(&list1, 10);
    listInsertToTail(&list1, 20);
    listInsertToTail(&list1, 30);
    listInsertToTail(&list1, 40);
    // *next(&list1, 2) = 300;
    listInsertToTail(&list1, 50);
    listInsertToTail(&list1, 45);

    listDelete(&list1, 3);

    listOptimize(&list1);

    listInsertToTail(&list1, 59);
    
    listDtor(&list1);
}