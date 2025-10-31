#include "list.h"
#include "general_list.h"
#include "general/file.h"
#include "general/poison.h"
#include "general/debug.h"

#include <malloc.h>
#include <assert.h>

#define verify(list) if(verifyList(list, __FUNCTION__, __FILE__, __LINE__) != PROCESS_OK) return list->status.type

static listStatus listInit(list_t* list, size_t startIndex = 1);
static listStatus realocateListMem(list_t* list);

listStatus listCtor(list_t* list){
    assert(list);
    assert(list->capacity > 2);

    list->size   = 0;
    *freeInd(list) = 1;

    list->elem = (listElem_t*) calloc(list->capacity, sizeof(listElem_t));
    assert(list->elem);
    
    listInit(list);
    
    list->status.type = PROCESS_OK;
    return PROCESS_OK;
}

listStatus listDtor(list_t* list){
    assert(list);

    poisonMemory(list->elem, sizeof(listElem_t) * list->capacity);
    free(list->elem);
    list->elem = NULL;

    poisonMemory(&list->size,        sizeof(list->size));
    poisonMemory(&list->capacity,    sizeof(list->capacity));
    poisonMemory(&*freeInd(list), sizeof(*freeInd(list)));
    poisonMemory(&list->status,      sizeof(list->status));

    return PROCESS_OK;
}

listStatus listInsertAfter(list_t* list, listVal_t insIndex, listVal_t insValue){
    assert(list);

    verify(list);
    log(list, "before", "insertAfter", insIndex);
    
    if((list->capacity - list->size) <= 2){
        realocateListMem(list);
    }

    *data(list, *freeInd(list)) = insValue;
    listVal_t insertedCellPhysInd = *freeInd(list);
    *freeInd(list) = *next(list, *freeInd(list));

    *next(list, insertedCellPhysInd) = *next(list, insIndex);
    *prev(list, insertedCellPhysInd) = insIndex;

    *prev(list, *next(list, insIndex)) = insertedCellPhysInd;
    *next(list, insIndex) = insertedCellPhysInd;

    *next(list, *tail(list)) = 0;
    *prev(list, *freeInd(list)) = *tail(list);

    (list->size)++;

    verify(list);
    log(list, "after", "insertAfter", insIndex);

    return PROCESS_OK;
}

listStatus listInsertBefore(list_t* list, listVal_t insIndex, listVal_t insValue){
    assert(list);

    insIndex = *prev(list, insIndex);
    listInsertAfter(list, insIndex, insValue);

    return PROCESS_OK;
}

listStatus listInsertToTail(list_t* list, listVal_t insValue){
    assert(list);
    
    listInsertBefore(list, 0, insValue);

    return PROCESS_OK;
}

listStatus listInsertToHead(list_t* list, listVal_t insValue){
    assert(list);
    
    listInsertAfter(list, 0, insValue);

    return PROCESS_OK;
}

listStatus listDelete(list_t* list, listVal_t deleteIndex){
    assert(list);

    verify(list);
    log(list, "before", "delete", deleteIndex);

    if(deleteIndex     == *tail(list)){
        *tail(list) = *prev(list, deleteIndex);
    }
    else{
        *next(list, *prev(list, deleteIndex)) = *next(list, deleteIndex);
        *prev(list, *next(list, deleteIndex)) = *prev(list, deleteIndex);
    }

    *data(list, deleteIndex) = LIST_POISON;
    *next(list, deleteIndex) = *freeInd(list);
    *prev(list, deleteIndex) = *tail(list);

    *freeInd(list) = deleteIndex;
    *next(list, *tail(list)) = *freeInd(list);

    (list->size)--;

    verify(list);
    log(list, "after", "delete", deleteIndex);

    return PROCESS_OK;
}

static listStatus listInit(list_t* list, size_t startIndex){
    assert(list);

    static size_t initCount = 0;

    for(size_t fillInd = startIndex; fillInd < list->capacity; fillInd++){
        *data(list, (listVal_t) fillInd) = LIST_POISON;
        *next(list, (listVal_t) fillInd) = (listVal_t) fillInd + 1;
        *prev(list, (listVal_t) fillInd) = (listVal_t) fillInd - 1;
    }

    if(initCount == 0){
        *data(list, 0) = LIST_POISON;
        *head(list) = 0;
        *tail(list) = 0;
    }

    initCount++;

    return PROCESS_OK;
}


static listStatus realocateListMem(list_t* list){
    assert(list);

    static size_t reallocationCount = 0;

    verify(list);
    log(list, "before", "reallocation", reallocationCount);

    printf("difference: %lu\n", list->capacity - list->size);

    size_t initStartIndex = list->capacity;

    list->capacity = list->capacity * 2;
    listElem_t* temp = (listElem_t*) realloc(list->elem, list->capacity * sizeof(listElem_t));
    assert(temp);

    list->elem = temp;

    
    listInit(list, initStartIndex);
    
    reallocationCount++;

    verify(list);
    log(list, "after", "reallocation", reallocationCount);

    return PROCESS_OK;
}