#include "classicalList.h"
#include "../general/file.h"
#include "../general/poison.h"
#include "../general/debug.h"

#include <malloc.h>
#include <assert.h>

#define verify(list) if(verifyList(list, __FUNCTION__, __FILE__, __LINE__) != PROCESS_OK) return list->status.type

static listStatus listInit(list_t* list, size_t startIndex = 1);

listStatus listCtor(list_t* list){
    assert(list);

    list->size     = 0;

    listElem_t* dummy = (listElem_t*) calloc(1, sizeof(listElem_t));
    assert(dummy);

    list->dummy = dummy;
    list->dummy->next = dummy;
    list->dummy->prev = dummy;
    
    list->status.type = PROCESS_OK;
    return PROCESS_OK;
}

listStatus listDtor(list_t* list){
    assert(list);

    listElem_t* curCell = *head(list);
    while(curCell->next != *head(list)){
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

    return PROCESS_OK;
}

listStatus listInsertAfter(list_t* list, listElem_t* insAddr, listVal_t insValue){
    assert(list);

    verify(list);
    log(list, "before", "insertAfter", 333);

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

    *next(list, *tail(list)) = *head(list);

    printf("head(newElem): %p\n", *head(list));

    (list->size)++;

    verify(list);
    log(list, "after", "insertAfter", 333);

    return PROCESS_OK;
}

listStatus listInsertBefore(list_t* list, listElem_t* insAddr, listVal_t insValue){
    assert(list);

    insAddr = *prev(list, insAddr);
    listInsertAfter(list, insAddr, insValue);

    return PROCESS_OK;
}

listStatus listInsertToTail(list_t* list, listVal_t insValue){
    assert(list);
    
    listInsertBefore(list, list->dummy, insValue);

    return PROCESS_OK;
}

// listStatus listInsertToHead(list_t* list, listVal_t insValue){
//     assert(list);
    
//     listInsertAfter(list, 0, insValue);

//     return PROCESS_OK;
// }

// listStatus listDelete(list_t* list, listVal_t deleteIndex){
//     assert(list);

//     verify(list);
//     log(list, "before", "delete", deleteIndex);

//     if(deleteIndex == *tail(list)){
//         *tail(list) = *prev(list, deleteIndex);
//     }
//     else{
//         *next(list, *prev(list, deleteIndex)) = *next(list, deleteIndex);
//         *prev(list, *next(list, deleteIndex)) = *prev(list, deleteIndex);
//     }

//     *data(list, deleteIndex) = LIST_POISON;
//     *next(list, deleteIndex) = *freeInd(list);
//     *prev(list, deleteIndex) = *tail(list);

//     *freeInd(list) = deleteIndex;
//     *next(list, *tail(list)) = *freeInd(list);

//     *next(list, *tail(list)) = 0;

//     (list->size)--;

//     verify(list);
//     log(list, "after", "delete", deleteIndex);

//     return PROCESS_OK;
// }
