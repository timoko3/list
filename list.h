#ifndef LIST_H
#define LIST_H

#include "protection_list.h"
#include "general_list.h"

listStatus listCtor(list_t* list);
listStatus listDtor(list_t* list);

listStatus listInsertAfter(list_t* list, listVal_t index, listVal_t insValue);
listStatus listInsertBefore(list_t* list, listVal_t insIndex, listVal_t insValue);
listStatus listInsertToHead(list_t* list, listVal_t insValue);
listStatus listInsertToTail(list_t* list, listVal_t insValue);
listStatus listDelete(list_t* list, listVal_t index);

#endif /* LIST_H */