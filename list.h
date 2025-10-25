#include <stddef.h>
#include <limits.h>

typedef int listVal_t;

const listVal_t LIST_POISON = INT_MAX;

enum listStatus{
    PROCESS_OK
};

struct listElem_t{
    listVal_t data;
    size_t    next;
    size_t    prev;
};

struct list_t{
    listElem_t* elem;
    size_t      size;
    size_t      capacity;
    listStatus  status;
};

listStatus listCtor(list_t* lst);
listStatus listDtor(list_t* lst);

void listDumpBasic(list_t* lst);
listStatus listAdd(list_t* lst, listVal_t addValue);
listStatus listInsertAfter(list_t* lst, size_t index, listVal_t insValue);