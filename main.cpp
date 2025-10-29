#include "list.h"

#include <stdio.h>

int main(void){
    list_t list1;
    list1.capacity = 10;

    listCtor(&list1);

    listDumpBasic(&list1);

    listInsertAfter(&list1, 0, 20);

    listInsertAfter(&list1, 0, 30);

    listInsertAfter(&list1, 0, 40);

    listInsertAfter(&list1, 0, 50);

    listInsertAfter(&list1, 0, 60);

    listInsertAfter(&list1, 2, 1000);

    listGraphDump(&list1);

    listDumpBasic(&list1);

    listDumpBasic(&list1);

    

    listDtor(&list1);
}