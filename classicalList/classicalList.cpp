#include "classicalList.h"
#include "../general/file.h"
#include "../general/poison.h"
#include "../general/debug.h"

#include <stdint.h>
#include <malloc.h>
#include <assert.h>

#define verify(list) if(verifyList(list, __FUNCTION__, __FILE__, __LINE__) != PROCESS_OK) return NULL

curAnchorNode listCtor(list_t* list){
    assert(list);

    list->size     = 0;

    listElem_t* dummy = (listElem_t*) calloc(1, sizeof(listElem_t));
    assert(dummy);

    list->dummy = dummy;
    list->dummy->data = LIST_POISON;
    list->dummy->next = dummy;
    list->dummy->prev = dummy;
    
    return list->dummy;
}

curAnchorNode listDtor(list_t* list){
    assert(list);

    listElem_t* curCell = *head(list);
    while(*data(list, curCell) != LIST_POISON){
        listElem_t* nextCell = *next(list, curCell);

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

curAnchorNode listInsertAfter(list_t* list, listElem_t* insAddr, listVal_t insValue){
    assert(list);

    verify(list);
    log(list, "before", "insertAfter", (long long)(uintptr_t) insAddr);
    
    listElem_t* newElem = (listElem_t*) calloc(1, sizeof(listElem_t));
    assert(newElem);
    printf("newElemAddr: %p\n", newElem);
    *data(list, newElem) = insValue;
    printf("next(insAddr): %p\n", *next(list, insAddr));
    *next(list, newElem) = *next(list, insAddr);
    *prev(list, newElem) = insAddr;
    printf("next(newElem): %p\n", *next(list, newElem));
    printf("prev(newElem): %p\n", *prev(list, newElem));

    *prev(list, *next(list, insAddr)) = newElem;
    *next(list, insAddr) = newElem;

    // *next(list, *tail(list)) = *head(list);

    printf("head(newElem): %p\n", *head(list));

    (list->size)++;

    verify(list);
    log(list, "after", "insertAfter", (long long)(uintptr_t) insAddr);

    return newElem;
}

curAnchorNode listInsertBefore(list_t* list, listElem_t* insAddr, listVal_t insValue){
    assert(list);

    insAddr = *prev(list, insAddr);
    curAnchorNode anchor = listInsertAfter(list, insAddr, insValue);

    return anchor;
}

curAnchorNode listInsertToTail(list_t* list, listVal_t insValue){
    assert(list);
    
    curAnchorNode anchor = listInsertBefore(list, list->dummy, insValue);

    return anchor;
}

curAnchorNode listInsertToHead(list_t* list, listVal_t insValue){
    assert(list);
    
    curAnchorNode anchor = listInsertAfter(list, list->dummy, insValue);

    return anchor;
}

curAnchorNode listDelete(list_t* list, listElem_t* deleteAddr){
    assert(list);

    long long logParam = (long long)(uintptr_t) deleteAddr;

    verify(list);
    log(list, "before", "delete", logParam);

    *next(list, *prev(list, deleteAddr)) = *next(list, deleteAddr);
    *prev(list, *next(list, deleteAddr)) = *prev(list, deleteAddr);

    poisonMemory(deleteAddr, sizeof(*deleteAddr));
    free(deleteAddr);

    *next(list, *tail(list)) = list->dummy;

    (list->size)--;

    verify(list);
    log(list, "after", "delete", logParam);

    return *tail(list);
}
