#ifndef CLASSICAL_LIST_H
#define CLASSICAL_LIST_H

#include "protection_classical_list.h"
#include "general_classical_list.h"

// #define DEBUG

typedef listClassicalElem_t* curAnchorNode;

curAnchorNode listClassicalCtor(listClassical_t* list);
curAnchorNode listClassicalDtor(listClassical_t* list);

curAnchorNode listClassicalInsertAfter (listClassical_t* list, listClassicalElem_t* index,    listVal_t insValue);
curAnchorNode listClassicalInsertBefore(listClassical_t* list, listClassicalElem_t* insIndex, listVal_t insValue);
curAnchorNode listClassicalInsertToHead(listClassical_t* list, listVal_t insValue);
curAnchorNode listClassicalInsertToTail(listClassical_t* list, listVal_t insValue);
curAnchorNode listClassicalDelete      (listClassical_t* list, listClassicalElem_t* deleteIndex);

#endif /* CLASSICAL_LIST_H */