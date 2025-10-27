#include "list.h"
#include "general/file.h"
#include "general/poison.h"

#include <malloc.h>
#include <assert.h>

const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDump.dot";

#define $ fprintf(stderr, "MEOW in %s:%d\n", __FILE__, __LINE__);

const size_t MAX_NODE_NAME_SIZE = 20;

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

void listDump(list_t* lst){
    assert(lst);

    fileDescription graphDump = {
        GRAPH_DUMP_DOT_FILE_NAME,
        "wb"
    };

    $
    FILE* graphFilePtr = myOpenFile(&graphDump);
    assert(graphFilePtr);

    fprintf(graphFilePtr, "digraph G {\n");
    fprintf(graphFilePtr, "\tnode [shape=ellipse, style=filled, fillcolor=\"lightgray\"];\n\n");

    size_t* dumpNodes = (size_t*) calloc(lst->capacity, sizeof(size_t));
    size_t nodeInd = 1;
    for(size_t curCellInd = lst->elem->next; lst->elem[curCellInd].prev != lst->elem->prev; curCellInd = lst->elem[curCellInd].next){

        fprintf(graphFilePtr, "\tnode%lu [label=\"%d\"];\n", nodeInd, lst->elem[curCellInd].data);
        
        dumpNodes[curCellInd] = nodeInd;
        nodeInd++;
    }
    fprintf(graphFilePtr, "\n");

    fprintf(graphFilePtr, "\t{ rank=same; ");

    size_t curNodeInd = 1;
    while(curNodeInd < nodeInd){
        if(dumpNodes[curNodeInd] == 0){
            continue;
        }

        fprintf(graphFilePtr, "node%lu; ", dumpNodes[curNodeInd]);
        curNodeInd++;
    }
    fprintf(graphFilePtr, "}\n\n");

    fprintf(graphFilePtr, "\t");
    for(size_t curCellInd = lst->elem->next; lst->elem[curCellInd].prev != lst->elem->prev; curCellInd = lst->elem[curCellInd].next){

        fprintf(graphFilePtr, "node%lu", curCellInd);
        
        if(curCellInd != lst->elem->prev){
            fprintf(graphFilePtr, " -> ");
        }
    }
    fprintf(graphFilePtr, " [style = invis];\n\n");

    
    fprintf(graphFilePtr, "\t");
    for(curNodeInd = 1; curNodeInd < nodeInd; curNodeInd++){
        fprintf(graphFilePtr, "node%lu", curNodeInd);
        if(nodeInd - curNodeInd > 1){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, ";");
        }
    }

    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    free(dumpNodes);
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

