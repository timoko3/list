
#include "auxilary_list.h"
#include "protection_list.h"
#include "general/file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDump.dot";
const size_t MAX_NODE_NAME_SIZE      = 20;

const char* DIRECT_CHAIN_COLOR  = "#98FB98";
const char* REVERSE_CHAIN_COLOR = "#DC143C";
const char* FREE_CHAIN_COLOR    = "#6A5ACD";

void listDumpBasic(list_t* list){
    assert(list);

    printf("listGraphDump:\n");

    printf("\t*head(list): %lu\n", *head(list));
    printf("\t*tail(list): %lu\n", *tail(list));
    printf("\tcurFreeElem: %lu\n", *freeInd(list));    

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
            fprintf(graphFilePtr, "node0 [label=\"head = %lu | tail = %lu | free_tail = %lu\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", *head(list), *tail(list), *freeInd(list));
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
    fprintf(graphFilePtr, "free_head_label -> node%lu [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *freeInd(list), FREE_CHAIN_COLOR);
    
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
    for(size_t curCellInd = *freeInd(list); curCellInd < list->capacity; curCellInd = *next(list, curCellInd)){
        fprintf(graphFilePtr, "node%lu", curCellInd);
        if(*next(list, curCellInd) != list->capacity){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=2, weight=100000];\n", FREE_CHAIN_COLOR);
        }
    }

    fprintf(graphFilePtr, "\n");
    for(size_t curCellInd = *freeInd(list); curCellInd < list->capacity; curCellInd = *next(list, curCellInd)){
        
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