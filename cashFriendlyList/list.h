#ifndef LIST_H
#define LIST_H

#include "protection_list.h"
#include "general_list.h"

// #define DEBUG

listStatus listCtor(list_t* list);
listStatus listDtor(list_t* list);

listStatus listInsertAfter(list_t* list, listVal_t index, listVal_t insValue);
listStatus listInsertBefore(list_t* list, listVal_t insIndex, listVal_t insValue);
listStatus lisInsertToHead(list_t* list, listVal_t insValue);
listStatus listInsertToTail(list_t* list, listVal_t insValue);
listStatus listDelete(list_t* list, listVal_t index);

listStatus listOptimize(list_t* list);
listStatus listLinearize(list_t* list);
listStatus listFreeUnusedMem(list_t* list);

#endif /* LIST_H */