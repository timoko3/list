#include "protection_classical_list.h"
#include "../general/file.h"
#include "../general/debug.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDumps/graphDump.dot";
const char* HTML_LOG_FILE_NAME       = "logList.html";
const listVal_t MAX_NODE_NAME_SIZE   = 20;
const char* CONVERT_TO_IMAGE_COMMAND = "dot -Tpng graphDumps/graphDump.dot -o graphDumps/graph%d.png";
const size_t CONVERSION_COMMAND_SIZE = 61;

const char* DIRECT_CHAIN_COLOR  = "#6A5ACD";
const char* REVERSE_CHAIN_COLOR = "#c75a96ff";
const char* BORDER_CHAIN_COLOR  = "#ffe9b5ff";
const char* FREE_CHAIN_COLOR    = "#326932";
const char* FREE_NODE_FILLCOLOR = "#16c56eff";
const char* FREE_NODE_FONTCOLOR = "#222926ff";


static size_t logCount = 0;
static double SCALE_KOEF = 20;

static void assignErrorStruct(list_t* list, listStatus type);

static bool connectivityCheck(list_t* list);
static bool addressValidityCheck(list_t* list);

listStatus verifyList(list_t* list, const char* function, const char* file, const int line){
    if(list == NULL){
        printf("list — нулевой указатель\n");  
    } 
    else{
        if(list->dummy == NULL){  
            assignErrorStruct(list, NULL_POINTER);
            printf("dummy — нулевой указатель\n");
        }
        else if(!addressValidityCheck(list)){
            assignErrorStruct(list, NON_VALID_INDEXES);
        }
        // else if(!connectivityCheck(list)){
        //     assignErrorStruct(list, LIST_NOT_CONNECTED);
        // }

        else{
            assignErrorStruct(list, PROCESS_OK);
            return PROCESS_OK;
        }
    }
    printf("%s\n", list->status.text);
    htmlLog(list, file, function, line, "error", "verification", -1);
    return list->status.type;
}

static void assignErrorStruct(list_t* list, listStatus type){
    assert(list);

    for(size_t curErrInd = 0; curErrInd < sizeof(listStatuses) / sizeof(listStatusDescription); curErrInd++){
        if(listStatuses[curErrInd].type == type){
            list->status = listStatuses[curErrInd];
        }
    }
}

static bool addressValidityCheck(list_t* list){
    assert(list);
    $
    for(listElem_t* curCell = *head(list); curCell != *tail(list); curCell = *next(list, curCell)){
        if(*next(list, curCell) == NULL || *prev(list, curCell) == NULL){
            return false;
        }
    }

    return true;
}

// static bool connectivityCheck(list_t* list){
//     assert(list);

//     size_t connectionsCount = 0;
//     for(listElem_t* curCell = *head(list); *next(list, curCell) != list->dummy; curCell = *next(list, curCell)){
//         connectionsCount++;
//     }
//     size_t referenceConnectionsCount = 0;

//     if(list->size != 0){
//         referenceConnectionsCount = list->size - 1;
//     }

//     fprintf(stdout, "connectionsCount: %lu, size: %lu\n", connectionsCount, referenceConnectionsCount);
//     if(connectionsCount != referenceConnectionsCount){
//         return false;
//     }
//     return true;
// }

void htmlLog(list_t* list, const char* callFileName, const char* callFuncName, int callLine, 
                           const char* callCase, const char* actionName, listVal_t parameter){
    assert(list);
    assert(callFileName);
    assert(callFuncName);
    assert(callCase);
    assert(actionName);


    fileDescription logFile = {};
    if(logCount == 0){
        logFile = {
            HTML_LOG_FILE_NAME,
            "wb"
        };
    }
    else{
        logFile = {
            HTML_LOG_FILE_NAME,
            "ab"
        };
    }
    $
    FILE* logFilePtr = myOpenFile(&logFile);
    assert(logFilePtr);
    $

    fprintf(logFilePtr, "<pre>\n");

    fprintf(logFilePtr, "<h3> DUMP <font color = red> %s </font> %s (%d) </h3>\n", callCase, actionName, parameter);

    if(parameter == -1){
        fprintf(logFilePtr, "<font color = red>%s</font>", list->status.text);
    }

    fprintf(logFilePtr, "In file %s at %s:%d\n", callFileName, callFuncName, callLine);


    fprintf(logFilePtr, "dump:\n");

    listDumpBasic(list, logFilePtr);

    fprintf(logFilePtr, "graphDump:\n");
    $
    listGraphDump(list);
    $
    fprintf(logFilePtr, "\n\n <img src=graphDumps/graph%lu.png style=\"width: %lf%%; height: auto;\">\n", logCount, SCALE_KOEF + list->size * SCALE_KOEF);

    fprintf(logFilePtr, "\n----------------------------------------------------------------------------\n");

    fclose(logFilePtr);
}

void listDumpBasic(list_t* list, FILE* stream){
    assert(list);
    assert(stream);

    fprintf(stream, "listDump:\n");

    fprintf(stream, "\tdummyAddr: %p\n",     list->dummy);
    fprintf(stream, "\tsize: %lu\n",         list->size);
    fprintf(stream, "\thead: %p\n",          *head(list));
    fprintf(stream, "\ttail: %p\n",          *tail(list));

    fprintf(stream, "\telements:\n");

    for(listElem_t* curCell = *head(list); *data(list, curCell) != LIST_POISON; curCell = *next(list, curCell)){
        fprintf(stream, "\t\taddr: %p, data: %-3d, next: %-3p, prev: %-3p\n", 
                curCell,
                *data(list, curCell), 
                *next(list, curCell), 
                *prev(list, curCell));                                                                  
}
}

void listGraphDump(list_t* list){
    assert(list);

    logCount++;    
    $
    fileDescription graphDump = {
        GRAPH_DUMP_DOT_FILE_NAME,
        "wb"
    };
    FILE* graphFilePtr = myOpenFile(&graphDump);
    assert(graphFilePtr);
    $

    printf("curCell = %p, dummy = %p\n", *next(list, *next(list, list->dummy)), list->dummy);
    fprintf(graphFilePtr, "digraph G {\n");
    fprintf(graphFilePtr, "rankdir=LR\n");
    fprintf(graphFilePtr, "bgcolor=\"transparent\"\n");

    fprintf(graphFilePtr, "nodesep = 1;\n");
    fprintf(graphFilePtr, "ranksep = 1\n");
    
    fprintf(graphFilePtr, "\tnode [shape=record, style=\"filled\", fillcolor=\"#FFA089\", fontcolor=\"black\", color=\"#007CAD\", penwidth=2.5, fontname=\"Tahoma\", fontsize=25];\n\n");
    // fprintf(graphFilePtr, "edge [color=\"#2d714f\", arrowsize=1, penwidth=5, arrowhead=\"vee\", style=\"bold\"];\n");
    

    fprintf(graphFilePtr, "node%d [label=\"address = %p | data = PZN | {head = %p | tail = %p} \", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", list->dummy, list->dummy, *head(list), *tail(list));
    
    for(listElem_t* curCell = *head(list); *data(list, curCell) != LIST_POISON; curCell = *next(list, curCell)){
        fprintf(graphFilePtr, "\tnode%d [label=\"address = %p | data = %d | {prev = %p | next = %p} \"];\n", curCell, curCell, *data(list, curCell), *prev(list, curCell), *next(list, curCell));
    }
    fprintf(graphFilePtr, "\n"); 
    
    fprintf(graphFilePtr, "head_label      [shape=box, width = 2.4, height = 1.4, label=\"HEAD\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      [shape=box, width = 2.4, height = 1.4, label=\"TAIL\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", REVERSE_CHAIN_COLOR);


    fprintf(graphFilePtr, "head_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *head(list),    DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *tail(list),    REVERSE_CHAIN_COLOR);
    $
    // установка нодов по индексам
    fprintf(graphFilePtr, "\t");
    fprintf(graphFilePtr, "node%d -> node%d[style=invis, weight = 100000];\n", list->dummy, *head(list));
    for(listElem_t* curCell = *head(list); *data(list, curCell) != LIST_POISON && list->size > 1; curCell = *next(list, curCell)){
        fprintf(graphFilePtr, "node%d", curCell);
        if(*data(list, *next(list, curCell)) != LIST_POISON){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[style=invis, weight=1000000];\n");
        }
    }
$

    // if(list->status.type == NON_VALID_INDEXES){
    //     for(size_t curCellInd = 0; curCellInd < list->capacity; curCellInd++){
    //         if(*next(list, curCellInd) > list->capacity){
    //             fprintf(graphFilePtr, "\tnode%d [label=\"phys   Ind = %d\", shape=doubleoctagon, fillcolor = \"red\", fontcolor=\"white\", color=\"#007CAD\", penwidth=3, fontname=\"Tahoma Bold\", fontsize=40];\n", *next(list, curCellInd), *next(list, curCellInd));
    //         }
    //     }
    // }

    fprintf(graphFilePtr, "\t");

    printf("head: %p, tail: %p\n", *head(list), *tail(list));
    for(listElem_t* curCell = *head(list); curCell != list->dummy; curCell = *next(list, curCell)){
        if(*next(list, curCell) == list->dummy){
            fprintf(graphFilePtr, "node%d [fillcolor = \"%s:%s\", fontcolor = \"%s\"]\n", curCell,              DIRECT_CHAIN_COLOR , REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);
            break;
        }
            
        fprintf(graphFilePtr, "node%d [fillcolor = \"%s:%s\", fontcolor = \"%s\"]\n", curCell,              DIRECT_CHAIN_COLOR , REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);

        fprintf(graphFilePtr, "node%d", curCell);
        
        fprintf(graphFilePtr, " -> ");

        fprintf(graphFilePtr, "node%d", *next(list, curCell));

        fprintf(graphFilePtr, "[color=\"%s:%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false, dir = both];\n", DIRECT_CHAIN_COLOR, REVERSE_CHAIN_COLOR);
    }

    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    
    char convertToImageCommandString[CONVERSION_COMMAND_SIZE];

    sprintf(convertToImageCommandString, CONVERT_TO_IMAGE_COMMAND, logCount);   

    system((const char*) convertToImageCommandString);
}