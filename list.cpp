#include "list.h"
#include "general/file.h"
#include "general/poison.h"

#include <stdlib.h>
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
    next(tail) = freeIndex;

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
    fprintf(graphFilePtr, "rankdir=LR\n");
    fprintf(graphFilePtr, "bgcolor=\"transparent\"\n");

    fprintf(graphFilePtr, "\tnode [shape=record, style=\"filled\", fillcolor=\"#FFA089\", fontcolor=\"black\", color=\"#007CAD\", penwidth=2.5, fontname=\"Tahoma\", fontsize=25];\n\n");
    fprintf(graphFilePtr, "edge [color=\"#2d714f\", arrowsize=1, penwidth=5, arrowhead=\"vee\", style=\"bold\"];\n");
    // fprintf(graphFilePtr, "params [label=\"head = %lu | tail = %lu | free_tail = %lu\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", head, tail, freeIndex);
    
    size_t headNodeInd = 0;
    size_t tailNodeInd = 0;
    for(size_t curCellInd = 0; curCellInd < list->capacity; curCellInd++){
        if(curCellInd == 0){
            fprintf(graphFilePtr, "node0 [label=\"head = %lu | tail = %lu | free_tail = %lu\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", head, tail, freeIndex);
            continue;
        }

        fprintf(graphFilePtr, "\tnode%lu [label=\"phys idx = %lu | data = %d | {prev = %lu | next = %lu} \"];\n", curCellInd, curCellInd, data(curCellInd), prev(curCellInd), next(curCellInd));

        if(curCellInd == head){
            headNodeInd = curCellInd;
        }
        else if(curCellInd == tail){
            tailNodeInd = curCellInd;  
        }
        
    }
    fprintf(graphFilePtr, "\n");
    
    fprintf(graphFilePtr, "head_label [shape=box, label=\"HEAD\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"lime\", fontcolor=\"darkblue\"];\n");
    fprintf(graphFilePtr, "tail_label [shape=box, label=\"TAIL\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"red\", fontcolor=\"darkblue\"];\n");
    fprintf(graphFilePtr, "free_head_label [shape=box, label=\"FREE\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"purple\", fontcolor=\"darkblue\"];\n");


    fprintf(graphFilePtr, "head_label -> node%lu [color=\"lime\", arrowsize=2.5, penwidth=3];\n", head);
    fprintf(graphFilePtr, "tail_label -> node%lu [color=\"red\", arrowsize=2.5, penwidth=3];\n", tail);
    fprintf(graphFilePtr, "free_head_label -> node%lu [color=\"purple\", arrowsize=2.5, penwidth=3];\n", freeIndex);
    
    // установка нодов по индексам
    fprintf(graphFilePtr, "node0 -> node1[style=invis, weight = 100000]");
    fprintf(graphFilePtr, "\t");
    for(size_t curCellInd = 1; curCellInd < list->capacity; curCellInd++){
        fprintf(graphFilePtr, "node%lu", curCellInd);
        if(curCellInd != list->capacity - 1){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[style=invis, weight=1000000];\n");
        }
    }

    /// установка связи между занятыми элементами списка от head
    fprintf(graphFilePtr, "\t");
    for(size_t curCellInd = head; (prev(curCellInd) != tail) && (data(curCellInd) != LIST_POISON); curCellInd = next(curCellInd)){
        fprintf(graphFilePtr, "node%lu", curCellInd);
        if(curCellInd != tail){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"lime\", arrowsize=1.5, penwidth=2, weight=100000];\n");
        }
    }

    for(size_t curCellInd = tail; (next(curCellInd) != head) || (data(curCellInd) != LIST_POISON); curCellInd = prev(curCellInd)){
        fprintf(graphFilePtr, "node%lu", curCellInd);
        if(curCellInd != head){
            fprintf(graphFilePtr, " -> ");
        }
            fprintf(graphFilePtr, "[color=\"red\", arrowsize=1.5, penwidth=2, weight=100000];\n");
    }

    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    
    system("dot -Tpng graphDump.dot -o graph.png");
}

static listStatus listInit(list_t* list){
    assert(list);

    for(size_t fillInd = 1; fillInd < list->capacity; fillInd++){
        data(fillInd) = LIST_POISON;
        next(fillInd) = fillInd + 1;
        prev(fillInd) = fillInd - 1;
    }

    data(0) = LIST_POISON;
    head = 0;
    tail = 0;

    return PROCESS_OK;
}

