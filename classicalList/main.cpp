#include "classicalList.h"

#include <stdio.h>

int main(void){
    list_t list1;

    listCtor(&list1);

    listInsertToTail(&list1, 10);
    listInsertToTail(&list1, 20);
    listInsertToTail(&list1, 30);
    listInsertToTail(&list1, 40);

    listInsertToTail(&list1, 50);
    listInsertToTail(&list1, 45);
    

    listDtor(&list1);
}