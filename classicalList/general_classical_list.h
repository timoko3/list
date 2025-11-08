#ifndef GENERAL__CLASSICAL_LIST_H
#define GENERAL__CLASSICAL_LIST_H

#include <limits.h>
#include <stddef.h>

#define log(list1, callCase, actionName, parameter) htmlLog(list1, __FILE__, __FUNCTION__, __LINE__, callCase, actionName, parameter)

typedef int listVal_t;

const listVal_t LIST_CLASSICAL_POISON = INT_MAX;

enum listClassicalStatus{
    CLASSICAL_PROCESS_OK,
    CLASSICAL_NULL_POINTER,
    CLASSICAL_CAPACITY_EXCEEDS_LIMIT,
    CLASSICAL_BAD_MEMORY_ALLOCATION,
    CLASSICAL_CAPACITY_IS_ZERO,
    CLASSICAL_SIZE_EXCEEDS_CAPACITY,
    CLASSICAL_NON_VALID_INDEXES,
    CLASSICAL_LIST_NOT_CONNECTED
};

struct listClassicalStatusDescription{
    listClassicalStatus  type;
    const char* text;
};

static struct listClassicalStatusDescription listClassicalStatuses[]{ 
    {CLASSICAL_PROCESS_OK,                    "Все хорошо\n"},
    {CLASSICAL_CAPACITY_EXCEEDS_LIMIT,        "Значение capacity превышает максимально возможное\n"}, 
    {CLASSICAL_NULL_POINTER,                  "Указатели не должны быть нулевыми\n"},
    {CLASSICAL_CAPACITY_IS_ZERO,              "Capacity равно 0\n"}, 
    {CLASSICAL_BAD_MEMORY_ALLOCATION,         "Некорректное выделение памяти\n"},
    {CLASSICAL_NON_VALID_INDEXES,             "prev или next имеют значение не принадлежащее списку\n"},
    {CLASSICAL_LIST_NOT_CONNECTED,            "Список не соединен или соединен не верно\n"}
};

struct listClassicalElem_t{
    listVal_t            data;
    listClassicalElem_t* next;
    listClassicalElem_t* prev;
};

struct listClassical_t{
    listClassicalElem_t*   dummy;
    size_t                 size;

    listClassicalStatusDescription  status;
};

listVal_t*            data(listClassical_t* list, listClassicalElem_t* elem);
listClassicalElem_t** prev(listClassical_t* list, listClassicalElem_t* elem);
listClassicalElem_t** next(listClassical_t* list, listClassicalElem_t* elem);
listClassicalElem_t** head(listClassical_t* list);
listClassicalElem_t** tail(listClassical_t* list);

#endif /* GENERAL__CLASSICAL_LIST_H */