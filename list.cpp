#include "list.h"
#include "general/file.h"
#include "general/poison.h"

#include <stdlib.h>
#include <assert.h>

#define $ fprintf(stderr, "MEOW in %s:%d\n", __FILE__, __LINE__);

const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDump.dot";
const size_t MAX_NODE_NAME_SIZE      = 20;

const char* DIRECT_CHAIN_COLOR  = "#98FB98";
const char* REVERSE_CHAIN_COLOR = "#DC143C";
const char* FREE_CHAIN_COLOR    = "#6A5ACD";

static listStatus listInit(list_t* list);

static listVal_t* data(list_t* list, listVal_t index);
static listVal_t* next(list_t* list, listVal_t index);
static listVal_t* prev(list_t* list, listVal_t index);
static listVal_t* head(list_t* list);
static listVal_t* tail(list_t* list);
static listVal_t* freeT(list_t* list);

listStatus listCtor(list_t* list){
    assert(list);
    assert(list->capacity > 2);

    list->size   = 0;
    *freeT(list) = 1;

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
    poisonMemory(&*freeT(list), sizeof(*freeT(list)));
    poisonMemory(&list->status,      sizeof(list->status));

    return PROCESS_OK;
}

listStatus listInsertAfter(list_t* list, size_t insIndex, listVal_t insValue){
    assert(list);
    
    *data(list, *freeT(list)) = insValue;
    size_t insertedCellPhysInd = *freeT(list);
    *freeT(list) = *next(list, *freeT(list));

    *next(list, insertedCellPhysInd) = *next(list, insIndex);
    *prev(list, insertedCellPhysInd) = insIndex;

    *prev(list, *next(list, insIndex)) = insertedCellPhysInd;
    *next(list, insIndex) = insertedCellPhysInd;

    *next(list, *tail(list)) = *freeT(list);
    *prev(list, *freeT(list)) = *tail(list);

    return PROCESS_OK;
}

listStatus listDelete(list_t* list, size_t deleteIndex){
    assert(list);

    if(deleteIndex     == *tail(list)){
        *tail(list) = *prev(list, deleteIndex);
    }
    else if(deleteIndex == *head(list)){
        *head(list) = *next(list, deleteIndex);
        *prev(list, *head(list)) = 0;
    }
    else{
        *next(list, *prev(list, deleteIndex)) = *next(list, deleteIndex);
        *prev(list, *next(list, deleteIndex)) = *prev(list, deleteIndex);
    }

    *data(list, deleteIndex) = LIST_POISON;
    *next(list, deleteIndex) = *freeT(list);
    *prev(list, deleteIndex) = *tail(list);

    *freeT(list) = deleteIndex;
    *next(list, *tail(list)) = *freeT(list);

    return PROCESS_OK;
}


void listDumpBasic(list_t* list){
    assert(list);

    printf("listGraphDump:\n");

    printf("\t*head(list): %lu\n", *head(list));
    printf("\t*tail(list): %lu\n", *tail(list));
    printf("\tcurFreeElem: %lu\n", *freeT(list));    

    printf("\telements:\n");
    for(size_t curElemInd = 0; curElemInd < list->capacity; curElemInd++){
        printf("\t\t*data: %-10d, *next: %-3lu, *prev: %-3lu\n", *data(list, curElemInd), 
                                                              *next(list, curElemInd), 
                                                              *prev(list, curElemInd));
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
    
    size_t headNodeInd = 0;
    size_t tailNodeInd = 0;
    for(size_t curCellInd = 0; curCellInd < list->capacity; curCellInd++){
        if(curCellInd == 0){
            fprintf(graphFilePtr, "node0 [label=\"head = %lu | tail = %lu | free_tail = %lu\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", *head(list), *tail(list), *freeT(list));
            continue;
        }

        fprintf(graphFilePtr, "\tnode%lu [label=\"phys idx = %lu | *data = %d | {*prev = %lu | *next = %lu} \"];\n", curCellInd, curCellInd, *data(list, curCellInd), *prev(list, curCellInd), *next(list, curCellInd));

        if(curCellInd == *head(list)){
            headNodeInd = curCellInd;
        }
        else if(curCellInd == *tail(list)){
            tailNodeInd = curCellInd;  
        }
        
    }
    fprintf(graphFilePtr, "\n");
    
    fprintf(graphFilePtr, "head_label [shape=box, label=\"*head(list)\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"lime\", fontcolor=\"darkblue\"];\n", DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label [shape=box, label=\"*tail(list)\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\",   fontcolor=\"darkblue\"];\n", REVERSE_CHAIN_COLOR);
    fprintf(graphFilePtr, "free_head_label [shape=box, label=\"FREE\", style=\"filled\"  , fillcolor=\"#BBDDEE\", color=\"%s\",   fontcolor=\"darkblue\"];\n", FREE_CHAIN_COLOR);


    fprintf(graphFilePtr, "head_label      -> node%lu [color=\"lime\", arrowsize=2.5, penwidth=3];\n", *head(list), DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      -> node%lu [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *tail(list), REVERSE_CHAIN_COLOR);
    fprintf(graphFilePtr, "free_head_label -> node%lu [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *freeT(list), FREE_CHAIN_COLOR);
    
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

    /// установка связи между занятыми элементами списка от *head(list)
    fprintf(graphFilePtr, "\t");
    for(size_t curCellInd = *head(list); (*prev(list, curCellInd) != *tail(list)) && (*data(list, curCellInd) != LIST_POISON); curCellInd = *next(list, curCellInd)){
        fprintf(graphFilePtr, "node%lu", curCellInd);
        if(curCellInd != *tail(list)){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=2, weight=100000];\n", DIRECT_CHAIN_COLOR);
        }
    }

    //Подсветка рамки для зеленого 
    fprintf(graphFilePtr, "\n");
    for(size_t curCellInd = *head(list); (*prev(list, curCellInd) != *tail(list)) && (*data(list, curCellInd) != LIST_POISON); curCellInd = *next(list, curCellInd)){
        
        fprintf(graphFilePtr, "\tnode%lu[color = \"%s\", penwidth=4];\n", curCellInd, DIRECT_CHAIN_COLOR);

    }

    fprintf(graphFilePtr, "\n\t");
    for(size_t curCellInd = *freeT(list); curCellInd < list->capacity; curCellInd = *next(list, curCellInd)){
        fprintf(graphFilePtr, "node%lu", curCellInd);
        if(*next(list, curCellInd) != list->capacity){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=2, weight=100000];\n", FREE_CHAIN_COLOR);
        }
    }

    fprintf(graphFilePtr, "\n");
    for(size_t curCellInd = *freeT(list); curCellInd < list->capacity; curCellInd = *next(list, curCellInd)){
        
        fprintf(graphFilePtr, "\tnode%lu[color = \"%s\", penwidth=4];\n", curCellInd, FREE_CHAIN_COLOR);

    }

    // for(size_t curCellInd = *tail(list); (*next(list, curCellInd) != *head(list)) || (*data(list, curCellInd) != LIST_POISON); curCellInd = *prev(list, curCellInd)){
    //     fprintf(graphFilePtr, "node%lu", curCellInd);
    //     if(curCellInd != *head(list)){
    //         fprintf(graphFilePtr, " -> ");
    //     }
    //         fprintf(graphFilePtr, "[color=\"red\", arrowsize=1.5, penwidth=2, weight=100000];\n");
    // }

    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    
    system("dot -Tpng graphDump.dot -o graph.png");
}

static listStatus listInit(list_t* list){
    assert(list);

    for(size_t fillInd = 1; fillInd < list->capacity; fillInd++){
        *data(list, fillInd) = LIST_POISON;
        *next(list, fillInd) = fillInd + 1;
        *prev(list, fillInd) = fillInd - 1;
    }

    *data(list, 0) = LIST_POISON;
    *head(list) = 0;
    *tail(list) = 0;

    return PROCESS_OK;
}

static listVal_t* data(list_t* list, listVal_t index){
    return &list->elem[index].data;
}

static listVal_t* next(list_t* list, listVal_t index){
    return &list->elem[index].next;
}

static listVal_t* prev(list_t* list, listVal_t index){
    return &list->elem[index].prev;
}

static listVal_t* head(list_t* list){
    return &list->elem->next;
}

static listVal_t* tail(list_t* list){
    return &list->elem->prev;
}


static listVal_t* freeT(list_t* list){
    return &list->freeCellInd;
}