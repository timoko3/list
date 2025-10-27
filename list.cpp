#include "list.h"
#include "general/file.h"
#include "general/poison.h"

#include <malloc.h>
#include <assert.h>

#define $ fprintf(stderr, "MEOW in %s:%d\n", __FILE__, __LINE__);

#define head list->elem->next
#define tail list->elem->prev

#define data(index) list->elem[index].data
#define next(index) list->elem[index].next
#define prev(index) list->elem[index].prev

#define freeIndex   list->freeCellInd

const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDump.dot";
const size_t MAX_NODE_NAME_SIZE      = 20;

static listStatus listInit(list_t* list);

listStatus listCtor(list_t* list){
    assert(list);
    assert(list->capacity > 2);

    list->size        = 0;
    freeIndex = 1;

    list->elem = (listElem_t*) calloc(list->capacity, sizeof(listElem_t));
    assert(list->elem);
    
    listInit(list);
    
    list->status = PROCESS_OK;
    return PROCESS_OK;
}

listStatus listDtor(list_t* list){
    assert(list);

    poisonMemory(list->elem, sizeof(listElem_t) * list->capacity);
    free(list->elem);
    list->elem = NULL;

    poisonMemory(&list->size,        sizeof(list->size));
    poisonMemory(&list->capacity,    sizeof(list->capacity));
    poisonMemory(&freeIndex, sizeof(freeIndex));
    poisonMemory(&list->status,      sizeof(list->status));

    return PROCESS_OK;
}

listStatus listAdd(list_t* list, listVal_t addValue){
    assert(list);

    data(freeIndex) = addValue;
    prev(freeIndex) = tail;

    tail = freeIndex;
    freeIndex = next(freeIndex);

    return PROCESS_OK;
}

listStatus listInsertAfter(list_t* list, size_t insIndex, listVal_t insValue){
    assert(list);
    
    data(freeIndex) = insValue;
    size_t insertedCellPhysInd = freeIndex;
    freeIndex = next(freeIndex);

    next(insertedCellPhysInd) = next(insIndex);
    prev(insertedCellPhysInd) = insIndex;

    prev(next(insIndex)) = insertedCellPhysInd;
    next(insIndex) = insertedCellPhysInd;

    next(tail) = freeIndex;
    prev(freeIndex) = tail;

    return PROCESS_OK;
}

listStatus listDelete(list_t* list, size_t deleteIndex){
    assert(list);

    if(deleteIndex      == tail){
        tail = prev(deleteIndex);
    }
    else if(deleteIndex == head){
        head = next(deleteIndex);
        prev(head) = 0;
    }
    else{
        next(prev(deleteIndex)) = next(deleteIndex);

        prev(next(deleteIndex)) = prev(deleteIndex);

        
    }

    data(deleteIndex) = LIST_POISON;
    next(deleteIndex) = freeIndex;
    prev(deleteIndex) = tail;

    freeIndex = deleteIndex;

    return PROCESS_OK;
}


void listDumpBasic(list_t* list){
    assert(list);

    printf("listGraphDump:\n");

    printf("\thead: %lu\n", head);
    printf("\ttail: %lu\n", tail);
    printf("\tcurFreeElem: %lu\n", freeIndex);    

    printf("\telements:\n");
    for(size_t curElemInd = 0; curElemInd < list->capacity; curElemInd++){
        printf("\t\tdata: %-10d, next: %-3lu, prev: %-3lu\n", data(curElemInd), 
                                                              next(curElemInd), 
                                                              prev(curElemInd));
    }
}

void listGraphDump(list_t* list){
    assert(list);

    fileDescription graphDump = {
        GRAPH_DUMP_DOT_FILE_NAME,
        "wb"
    };

    FILE* graphFilePtr = myOpenFile(&graphDump);
    assert(graphFilePtr);

    fprintf(graphFilePtr, "digraph G {\n");
    fprintf(graphFilePtr, "\tnode [shape=ellipse, style=filled, fillcolor=\"lightgray\"];\n\n");

    size_t* dumpNodes = (size_t*) calloc(list->capacity, sizeof(size_t));
    size_t nodeInd = 1;
    for(size_t curCellInd = head; prev(curCellInd) != tail; curCellInd = next(curCellInd)){

        fprintf(graphFilePtr, "\tnode%lu [label=\"%d\"];\n", nodeInd, data(curCellInd));
        
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
    for(size_t curCellInd = head; prev(curCellInd) != tail; curCellInd = next(curCellInd)){

        fprintf(graphFilePtr, "node%lu", curCellInd);
        
        if(curCellInd != tail){
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
        data(fillInd) = LIST_POISON;
        next(fillInd) = fillInd + 1;
        prev(fillInd) = fillInd - 1;
    }

    data(0) = LIST_POISON;
    head = 1;
    tail = 1;

    return PROCESS_OK;
}

