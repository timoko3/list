#ifndef GENERAL_LIST_H
#define GENERAL_LIST_H

#include <limits.h>
#include <stddef.h>
#include <stdarg.h>

#define log(list1, text, ...) htmlLog(list1, __FILE__, __FUNCTION__, __LINE__, text, ##__VA_ARGS__)

typedef int listVal_t;

const listVal_t LIST_POISON = INT_MAX;

enum listStatus{
    PROCESS_OK,
    NULL_POINTER,
    CAPACITY_EXCEEDS_LIMIT,
    BAD_MEMORY_ALLOCATION,
    CAPACITY_IS_ZERO,
    SIZE_EXCEEDS_CAPACITY,
    NON_VALID_INDEXES,
    LIST_NOT_CONNECTED
};

struct listStatusDescription{
    listStatus  type;
    const char* text;
};

static struct listStatusDescription listStatuses[]{ 
    {PROCESS_OK,                    "Все хорошо\n"},
    {CAPACITY_EXCEEDS_LIMIT,        "Значение capacity превышает максимально возможное\n"}, 
    {NULL_POINTER,                  "Указатели не должны быть нулевыми\n"},
    {CAPACITY_IS_ZERO,              "Capacity равно 0\n"}, 
    {BAD_MEMORY_ALLOCATION,         "Некорректное выделение памяти\n"},
    {SIZE_EXCEEDS_CAPACITY,         "Размер списка превышает объем выделяемой памяти\n"},
    {NON_VALID_INDEXES,             "prev или next имеют значение не принадлежащее списку\n"},
    {LIST_NOT_CONNECTED,            "Список не соединен или соединен не верно\n"}
};

struct listElem_t{
    listVal_t data;
    listVal_t next;
    listVal_t prev;
};

struct list_t{
    listElem_t*            elem;
    listVal_t              freeCellInd;
    size_t                 size;
    size_t                 capacity;
    listStatusDescription  status;
};

listVal_t* data(list_t* list, listVal_t index);
listVal_t* next(list_t* list, listVal_t index);
listVal_t* prev(list_t* list, listVal_t index);
listVal_t* head(list_t* list);
listVal_t* tail(list_t* list);
listVal_t* freeInd(list_t* list);

#endif /* GENERAL_LIST_H */