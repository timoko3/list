#include "general_list.h"

#include <assert.h>

listVal_t* data(list_t* list, listVal_t index){
    assert(list);

    return &list->elem[index].data;
}

listVal_t* next(list_t* list, listVal_t index){
    assert(list);
    
    return &list->elem[index].next;
}

listVal_t* prev(list_t* list, listVal_t index){
    assert(list);
    
    return &list->elem[index].prev;
}

listVal_t* head(list_t* list){
    assert(list);

    return &list->elem->next;
}

listVal_t* tail(list_t* list){
    assert(list);

    return &list->elem->prev;
}

listVal_t* freeInd(list_t* list){
    assert(list);

    return &list->freeCellInd;
}