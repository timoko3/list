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

    list->freeCellInd = 1;

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

    lst->elem[lst->freeCellInd].data = addValue;
    lst->elem->prev = lst->freeCellInd;
    lst->freeCellInd = lst->elem[lst->freeCellInd].next;

    return PROCESS_OK;
}

listStatus listInsertAfter(list_t* lst, size_t insIndex, listVal_t insValue){
    assert(lst);

    lst->elem[lst->freeCellInd].data = insValue;

    size_t tempInd = lst->elem[insIndex].next;

    lst->elem[insIndex].next = lst->freeCellInd;

    size_t nextFree = lst->elem[lst->freeCellInd].next;

    lst->elem[lst->freeCellInd].next = tempInd;
    lst->elem[lst->freeCellInd].prev = insIndex;

    lst->elem[tempInd].prev = lst->freeCellInd;
    lst->freeCellInd = nextFree;
    lst->elem[lst->elem->prev].next = lst->freeCellInd;

    lst->elem[lst->freeCellInd].prev = lst->elem->prev;

    return PROCESS_OK;
}

listStatus listDelete(list_t* lst, size_t index){
    assert(lst);

    if(index == lst->elem->prev)lst->elem->prev = lst->elem[index].prev;
    else if(index == lst->elem->next) {
        lst->elem->next = lst->elem[index].next;
        lst->elem[lst->elem->next].prev = 0;
    }
    else{
        lst->elem[lst->elem[index].prev].next = lst->elem[index].next;

        lst->elem[lst->elem[index].next].prev = lst->elem[index].prev;

        lst->freeCellInd = index;
    }

    lst->elem[index].data = LIST_POISON;
    lst->elem[index].next = lst->freeCellInd;
    lst->elem[index].prev = lst->elem->prev;

    return PROCESS_OK;
}


void listDumpBasic(list_t* lst){
    assert(lst);

    printf("listDump:\n");

    printf("\thead: %lu\n", lst->elem->next);
    printf("\ttail: %lu\n", lst->elem->prev);
    printf("\tcurFreeElem: %lu\n", lst->freeCellInd);    

    printf("\telements:\n");
    for(size_t curElemInd = 0; curElemInd < lst->capacity; curElemInd++){
        printf("\t\tdata: %-10d, next: %-3lu, prev: %-3lu\n", lst->elem[curElemInd].data, lst->elem[curElemInd].next, lst->elem[curElemInd].prev);
    }
}

static listStatus listInit(list_t* list){
    assert(list);

    for(size_t fillInd = 1; fillInd < list->capacity; fillInd++){
        list->elem[fillInd].data = LIST_POISON;
        list->elem[fillInd].next = fillInd + 1;
        list->elem[fillInd].prev = fillInd - 1;
    }

    list->elem->data = LIST_POISON;
    list->elem->next = 1;
    list->elem->prev = 1;

    return PROCESS_OK;
}

