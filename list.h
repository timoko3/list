#ifndef LIST_H
#define LIST_H

#include "protection_list.h"
#include "auxilary_list.h"

listStatus listCtor(list_t* list);
listStatus listDtor(list_t* list);

listStatus listInsertAfter(list_t* list, listVal_t index, listVal_t insValue);
listStatus listDelete(list_t* list, listVal_t index);

#endif /* LIST_H */