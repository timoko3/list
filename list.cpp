#include "list.h"
#include "general_list.h"
#include "general/file.h"
#include "general/poison.h"

#include <malloc.h>
#include <assert.h>

#define $ fprintf(stderr, "MEOW in %s:%d\n", __FILE__, __LINE__);

static listStatus listInit(list_t* list);

listStatus listCtor(list_t* list){
    assert(list);
    assert(list->capacity > 2);

    list->size   = 0;
    *freeInd(list) = 1;

    list->elem = (listElem_t*) calloc(list->capacity, sizeof(listElem_t));
    assert(list->elem);
    
    listInit(list);
    
    list->status = PROCESS_OK;
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
    
    *data(list, *freeInd(list)) = insValue;
    listVal_t insertedCellPhysInd = *freeInd(list);
    *freeInd(list) = *next(list, *freeInd(list));

    *next(list, insertedCellPhysInd) = *next(list, insIndex);
    *prev(list, insertedCellPhysInd) = insIndex;

    *prev(list, *next(list, insIndex)) = insertedCellPhysInd;
    *next(list, insIndex) = insertedCellPhysInd;

    *next(list, *tail(list)) = *freeInd(list);
    *prev(list, *freeInd(list)) = *tail(list);

    return PROCESS_OK;
}

listStatus listDelete(list_t* list, listVal_t deleteIndex){
    assert(list);

    if(deleteIndex     == *tail(list)){
        *tail(list) = *prev(list, deleteIndex);
    }
    else if(deleteIndex == *head(list)){
        *head(list) = *next(list, deleteIndex);
        *prev(list, *head(list)) = 0;
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

    return PROCESS_OK;
}

static listStatus listInit(list_t* list){
    assert(list);

    for(size_t fillInd = 1; fillInd < list->capacity; fillInd++){
        *data(list, (listVal_t) fillInd) = LIST_POISON;
        *next(list, (listVal_t) fillInd) = (listVal_t) fillInd + 1;
        *prev(list, (listVal_t) fillInd) = (listVal_t) fillInd - 1;
    }

    *data(list, 0) = LIST_POISON;
    *head(list) = 0;
    *tail(list) = 0;

    return PROCESS_OK;
}

