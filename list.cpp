#include "list.h"
#include "general/poison.h"

#include <malloc.h>
#include <assert.h>

static listStatus listInit(list_t* list);

listStatus listCtor(list_t* list){
    assert(list);
    assert(list->capacity > 2);

    list->size = 0;
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

    poisonMemory(&list->size, sizeof(list->size));
    poisonMemory(&list->capacity, sizeof(list->capacity));
    poisonMemory(&list->status, sizeof(list->status));

    return PROCESS_OK;
}

listStatus listAdd(list_t* lst, listVal_t addValue){
    assert(lst);

    lst->elem[lst->elem->prev].data = addValue;
    (lst->elem->prev)++;

    return PROCESS_OK;
}

listStatus listInsertAfter(list_t* lst, size_t index, listVal_t insValue){
    assert(lst);



    return PROCESS_OK;
}

void listDumpBasic(list_t* lst){
    assert(lst);

    printf("listDump:\n\t");
    for(size_t curElemInd = 0; curElemInd < lst->capacity; curElemInd++){
        printf("data: %d, next: %lu, prev: %lu\n", lst->elem->data, lst->elem->next, lst->elem->prev);
    }
}

static listStatus listInit(list_t* list){
    assert(list);

    for(size_t fillInd = 1; fillInd < list->capacity; fillInd++){
        list->elem->data = LIST_POISON;
        list->elem->next = fillInd + 1;
        list->elem->prev = fillInd - 1;
    }

    list->elem->data = LIST_POISON;
    list->elem->next = 1;
    list->elem->prev = 1;

    return PROCESS_OK;
}

