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
    fprintf(graphFilePtr, "bgcolor=\"transparent\"\n");

    fprintf(graphFilePtr, "\tnode [shape=record, style=\"filled\", fillcolor=\"#FFA089\", fontcolor=\"black\", color=\"#007CAD\", penwidth=2.5, fontname=\"Tahoma\", fontsize=25];\n\n");
    fprintf(graphFilePtr, "edge [color=\"#2d714f\", arrowsize=1, penwidth=5, arrowhead=\"vee\", style=\"bold\"];\n");
    fprintf(graphFilePtr, "params [label=\"{head = %lu | tail = %lu | free_tail = %lu}\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", head, tail, freeIndex);

    size_t* dumpNodes = (size_t*) calloc(list->capacity, sizeof(size_t));
    
    size_t nodeInd = 1;
    size_t headNodeInd = 0;
    size_t tailNodeInd = 0;
    for(size_t curCellInd = head; (prev(curCellInd) != tail) && (data(curCellInd) != LIST_POISON); curCellInd = next(curCellInd)){
        $
        printf("ind: %lu, prev(ind): %lu\n",curCellInd, prev(curCellInd));
        fprintf(graphFilePtr, "\tnode%lu [label=\"{phys idx = %lu | data = %d | {prev = %lu | next = %lu} }\"];\n", nodeInd, curCellInd, data(curCellInd), prev(curCellInd), next(curCellInd));

        if(curCellInd == head){
            headNodeInd = nodeInd;
        }
        else if(curCellInd == tail){
            tailNodeInd = nodeInd;  
        }
        
        dumpNodes[curCellInd] = nodeInd;
        nodeInd++;
    }
    fprintf(graphFilePtr, "\n");
    
    fprintf(graphFilePtr, " head_label [shape=box, label=\"HEAD\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"yellow\", fontcolor=\"darkblue\"];\n");
    fprintf(graphFilePtr, " tail_label [shape=box, label=\"TAIL\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"yellow\", fontcolor=\"darkblue\"];\n");


    fprintf(graphFilePtr, "head_label -> node%lu [color=\"yellow\", arrowsize=2.5, penwidth=3];\n", headNodeInd);
    fprintf(graphFilePtr, "tail_label -> node%lu [color=\"yellow\", arrowsize=2.5, penwidth=3];\n", tailNodeInd);
    
    /// установка связи между нодами
    fprintf(graphFilePtr, "\t");
    for(size_t curNodeInd = 1; curNodeInd < nodeInd; curNodeInd++){
        fprintf(graphFilePtr, "node%lu", curNodeInd);
        if(nodeInd - curNodeInd > 1){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"lime\", arrowsize=1.5, penwidth=2];\n");
        }
    }
    /// установка node в нужном порядке
    fprintf(graphFilePtr, "{rank=same; params; node1;}\n");
    
    fprintf(graphFilePtr, "{rank=same; ");
    for(size_t curNodeInd = 1; curNodeInd < nodeInd; curNodeInd++){

        fprintf(graphFilePtr, "node%lu; ", curNodeInd);
        
    }
    fprintf(graphFilePtr, "}\n");

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

