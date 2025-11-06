#include "classicalList.h"

#include "../general/file.h"

#include <stdio.h>

int main(void){
    listClassical_t list1;

    fileDescription tempDeb{
        "tempDebug.txt",
        "wb"
    };

    FILE* tempDebug = myOpenFile(&tempDeb);

    listClassicalCtor(&list1);


    listClassicalInsertToTail(&list1, 10);
    listClassicalInsertToTail(&list1, 20);
    listClassicalInsertToTail(&list1, 30);
    curAnchorNode toDelete = listClassicalInsertToTail(&list1, 40);
    listClassicalInsertToTail(&list1, 50);
    listClassicalInsertToTail(&list1, 45);
    
    listClassicalDelete(&list1, toDelete);

    listDumpBasic(&list1, tempDebug);

    fclose(tempDebug);
    listClassicalDtor(&list1);
}