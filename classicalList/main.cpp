#include "classicalList.h"

#include "../general/file.h"

#include <stdio.h>

int main(void){
    list_t list1;

    fileDescription tempDeb{
        "tempDebug.txt",
        "wb"
    };

    FILE* tempDebug = myOpenFile(&tempDeb);

    listCtor(&list1);


    listInsertToTail(&list1, 10);
    listInsertToTail(&list1, 20);
    listInsertToTail(&list1, 30);
    curAnchorNode toDelete = listInsertToTail(&list1, 40);
    listInsertToTail(&list1, 50);
    listInsertToTail(&list1, 45);
    
    listDelete(&list1, toDelete);

    listDumpBasic(&list1, tempDebug);

    fclose(tempDebug);
    listDtor(&list1);
}