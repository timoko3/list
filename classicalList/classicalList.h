#ifndef LIST_H
#define LIST_H

#include "protection_classical_list.h"
#include "general_classical_list.h"

typedef listElem_t* curAnchorNode;

curAnchorNode listCtor(list_t* list);
curAnchorNode listDtor(list_t* list);

curAnchorNode listInsertAfter (list_t* list, listElem_t* index,    listVal_t insValue);
curAnchorNode listInsertBefore(list_t* list, listElem_t* insIndex, listVal_t insValue);
curAnchorNode listInsertToHead(list_t* list, listVal_t insValue);
curAnchorNode listInsertToTail(list_t* list, listVal_t insValue);
curAnchorNode listDelete      (list_t* list, listElem_t* deleteIndex);

#endif /* LIST_H */