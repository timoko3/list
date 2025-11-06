#include "general_classical_list.h"

#include <assert.h>

listVal_t* data(listClassical_t* list, listClassicalElem_t* elem){
    assert(list);
    assert(elem);

    return &elem->data;
}

listClassicalElem_t** next(listClassical_t* list, listClassicalElem_t* elem){
    assert(list);
    assert(elem);
    
    return &elem->next;
}

listClassicalElem_t** prev(listClassical_t* list, listClassicalElem_t* elem){
    assert(list);
    assert(elem);
    
    return &elem->prev;
}

listClassicalElem_t** head(listClassical_t* list){
    assert(list);

    return &list->dummy->next;
}

listClassicalElem_t** tail(listClassical_t* list){
    assert(list);

    return &list->dummy->prev;
}
