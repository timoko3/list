#include "classicalList.h"
#include "../general/file.h"
#include "../general/poison.h"
#include "../general/debug.h"

#include <stdint.h>
#include <malloc.h>
#include <assert.h>

#define verify(list) if(verifyList(list, __FUNCTION__, __FILE__, __LINE__) != CLASSICAL_PROCESS_OK) return NULL

curAnchorNode listClassicalCtor(listClassical_t* list){
    assert(list);

    list->size     = 0;

    listClassicalElem_t* dummy = (listClassicalElem_t*) calloc(1, sizeof(listClassicalElem_t));
    assert(dummy);

    list->dummy = dummy;
    list->dummy->data = LIST_CLASSICAL_POISON;
    list->dummy->next = dummy;
    list->dummy->prev = dummy;
    
    return list->dummy;
}

curAnchorNode listClassicalDtor(listClassical_t* list){
    assert(list);

    listClassicalElem_t* curCell = *head(list);
    while(*data(list, curCell) != LIST_CLASSICAL_POISON){
        listClassicalElem_t* nextCell = *next(list, curCell);

        poisonMemory(curCell, sizeof(*curCell));
        free(curCell);

        curCell = nextCell;
    }

    poisonMemory(list->dummy, sizeof(list->dummy));
    free(list->dummy);
    list->dummy = NULL;

    poisonMemory(&list->size, sizeof(list->size));
    poisonMemory(&list->status, sizeof(list->status));

    return NULL;
}

curAnchorNode listClassicalInsertAfter(listClassical_t* list, listClassicalElem_t* insAddr, listVal_t insValue){
    assert(list);

    #ifdef DEBUG
    verify(list);
    log(list, "before", "insertAfter", (long long)(uintptr_t) insAddr);
    #endif /* DEBUG */

    listClassicalElem_t* newElem = (listClassicalElem_t*) calloc(1, sizeof(listClassicalElem_t));
    assert(newElem);
    
    *data(list, newElem) = insValue;
    *next(list, newElem) = *next(list, insAddr);
    *prev(list, newElem) = insAddr;

    *prev(list, *next(list, insAddr)) = newElem;
    *next(list, insAddr) = newElem;


    (list->size)++;

    #ifdef DEBUG
    verify(list);
    log(list, "after", "insertAfter", (long long)(uintptr_t) insAddr);
    #endif /* DEBUG */

    return newElem;
}

curAnchorNode listClassicalInsertBefore(listClassical_t* list, listClassicalElem_t* insAddr, listVal_t insValue){
    assert(list);

    insAddr = *prev(list, insAddr);
    curAnchorNode anchor = listClassicalInsertAfter(list, insAddr, insValue);

    return anchor;
}

curAnchorNode listClassicalInsertToTail(listClassical_t* list, listVal_t insValue){
    assert(list);
    
    curAnchorNode anchor = listClassicalInsertBefore(list, list->dummy, insValue);

    return anchor;
}

curAnchorNode listClassicalInsertToHead(listClassical_t* list, listVal_t insValue){
    assert(list);
    
    curAnchorNode anchor = listClassicalInsertAfter(list, list->dummy, insValue);

    return anchor;
}

curAnchorNode listClassicalDelete(listClassical_t* list, listClassicalElem_t* deleteAddr){
    assert(list);

    long long logParam = (long long)(uintptr_t) deleteAddr;

    #ifdef DEBUG
    verify(list);
    log(list, "before", "delete", logParam);
    #endif /* DEBUG */

    *next(list, *prev(list, deleteAddr)) = *next(list, deleteAddr);
    *prev(list, *next(list, deleteAddr)) = *prev(list, deleteAddr);

    poisonMemory(deleteAddr, sizeof(*deleteAddr));
    free(deleteAddr);

    *next(list, *tail(list)) = list->dummy;

    (list->size)--;

    #ifdef DEBUG
    verify(list);
    log(list, "after", "delete", logParam);
    #endif /* DEBUG */

    return *tail(list);
}
