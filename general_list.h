#ifndef GENERAL_LIST_H
#define GENERAL_LIST_H

#include <limits.h>
#include <stddef.h>

#define log(list1, callCase, actionName, parameter) htmlLog(list1, __FILE__, __FUNCTION__, __LINE__, callCase, actionName, parameter)

typedef int listVal_t;

const listVal_t LIST_POISON = INT_MAX;

enum listStatus{
    PROCESS_OK
};

struct listElem_t{
    listVal_t data;
    listVal_t next;
    listVal_t prev;
};

struct list_t{
    listElem_t* elem;
    listVal_t   freeCellInd;
    size_t      size;
    size_t      capacity;
    listStatus  status;
};

listVal_t* data(list_t* list, listVal_t index);
listVal_t* next(list_t* list, listVal_t index);
listVal_t* prev(list_t* list, listVal_t index);
listVal_t* head(list_t* list);
listVal_t* tail(list_t* list);
listVal_t* freeInd(list_t* list);

#endif /* GENERAL_LIST_H */