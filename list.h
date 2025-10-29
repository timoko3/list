#include <stddef.h>
#include <limits.h>

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

listStatus listCtor(list_t* list);
listStatus listDtor(list_t* list);

void listDumpBasic(list_t* list);
void listGraphDump(list_t* list);
listStatus listInsertAfter(list_t* list, size_t index, listVal_t insValue);
listStatus listDelete(list_t* list, size_t index);