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
    listDumpBasic(&list1, tempDebug);
    listInsertToTail(&list1, 10);
    listDumpBasic(&list1, tempDebug);
    listInsertToTail(&list1, 20);
    listDumpBasic(&list1, tempDebug);
    listInsertToTail(&list1, 30);
    listDumpBasic(&list1, tempDebug);
    listInsertToTail(&list1, 40);
    listDumpBasic(&list1, tempDebug);
    listInsertToTail(&list1, 50);
    listDumpBasic(&list1, tempDebug);
    listInsertToTail(&list1, 45);
    

    listDumpBasic(&list1, tempDebug);

    fclose(tempDebug);
    listDtor(&list1);
}