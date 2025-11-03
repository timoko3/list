#include "general_classical_list.h"

#include <assert.h>

listVal_t* data(list_t* list, listElem_t* elem){
    assert(list);
    assert(elem);

    return &elem->data;
}

listElem_t* next(list_t* list, listElem_t* elem){
    assert(list);
    assert(elem);
    
    return elem->next;
}

listElem_t* prev(list_t* list, listElem_t* elem){
    assert(list);
    assert(elem);
    
    return elem->prev;
}

listElem_t* head(list_t* list){
    assert(list);

    return list->head;
}

listElem_t* tail(list_t* list){
    assert(list);

    return list->tail;
}
