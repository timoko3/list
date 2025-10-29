#include "list.h"

#include <stdio.h>

int main(void){
    list_t list1;
    list1.capacity = 10;

    listCtor(&list1);


    listDumpBasic(&list1);

    listGraphDump(&list1);


    listDumpBasic(&list1);

    listDelete(&list1, 3);

    listDumpBasic(&list1);

    listDumpBasic(&list1);

    

    listDtor(&list1);
}