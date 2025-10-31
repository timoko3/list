#include "protection_list.h"
#include "general/file.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDumps/graphDump.dot";
const char* HTML_LOG_FILE_NAME       = "logList.html";
const listVal_t MAX_NODE_NAME_SIZE   = 20;
const char* CONVERT_TO_IMAGE_COMMAND = "dot -Tpng graphDumps/graphDump.dot -o graphDumps/graph%d.png";
const size_t CONVERSION_COMMAND_SIZE = 61;

const char* DIRECT_CHAIN_COLOR  = "#98FB98";
const char* REVERSE_CHAIN_COLOR = "#DC143C";
const char* FREE_CHAIN_COLOR    = "#6A5ACD";

static size_t logCount   = 0;

void htmlLog(list_t* list, const char* callFileName, const char* callFuncName, int callLine, 
                           const char* callCase, const char* actionName, listVal_t parameter){
    assert(list);
    assert(callFileName);
    assert(callFuncName);


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

    FILE* logFilePtr = myOpenFile(&logFile);
    assert(logFilePtr);

    fprintf(logFilePtr, "<pre>\n");

    fprintf(logFilePtr, "<h3> DUMP <font color = red> %s </font> %s (%d) </h3>\n", callCase, actionName, parameter);
    fprintf(logFilePtr, "In file %s at %s:%d\n", callFileName, callFuncName, callLine);

    fprintf(logFilePtr, "dump:\n");

    listDumpBasic(list, logFilePtr);

    fprintf(logFilePtr, "graphDump:\n");

    listGraphDump(list);

    fprintf(logFilePtr, "\n\n <img src=graphDumps/graph%d.png style=\"width: 85%%; height: auto;\">\n", logCount);

    fprintf(logFilePtr, "\n----------------------------------------------------------------------------\n");

    fclose(logFilePtr);
}

void listDumpBasic(list_t* list, FILE* stream){
    assert(list);
    assert(stream);

    fprintf(stream, "listDump:\n");

    fprintf(stream, "\thead: %d\n",        *head(list));
    fprintf(stream, "\ttail: %d\n",        *tail(list));
    fprintf(stream, "\tcurFreeElem: %d\n", *freeInd(list));    

    fprintf(stream, "\telements:\n");
    for(listVal_t curElemInd = 0; curElemInd < (listVal_t) list->capacity; curElemInd++){
        fprintf(stream, "\t\tdata: %-10d, next: %-3d, prev: %-3d\n", *data(list, (listVal_t) curElemInd), 
                                                                     *next(list, (listVal_t) curElemInd), 
                                                                     *prev(list, (listVal_t) curElemInd));
    }
}

void listGraphDump(list_t* list){
    assert(list);

    logCount++;    

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
    
    for(listVal_t curCellInd = 0; curCellInd < (listVal_t) list->capacity; curCellInd++){
        if(curCellInd == 0){
            fprintf(graphFilePtr, "node0 [label=\"head = %d | tail = %d | free_tail = %d\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", *head(list), *tail(list), *freeInd(list));
            continue;
        }

        fprintf(graphFilePtr, "\tnode%d [label=\"phys idx = %d | data = %d | {prev = %d | next = %d} \"];\n", curCellInd, curCellInd, *data(list, curCellInd), *prev(list, curCellInd), *next(list, curCellInd));
        
    }
    fprintf(graphFilePtr, "\n");
    
    fprintf(graphFilePtr, "head_label      [shape=box, label=\"HEAD\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"lime\", fontcolor=\"darkblue\"];\n");
    fprintf(graphFilePtr, "tail_label      [shape=box, label=\"TAIL\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\",   fontcolor=\"darkblue\"];\n", REVERSE_CHAIN_COLOR);
    fprintf(graphFilePtr, "free_head_label [shape=box, label=\"FREE\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\",   fontcolor=\"darkblue\"];\n", FREE_CHAIN_COLOR);


    fprintf(graphFilePtr, "head_label      -> node%d [color=\"lime\", arrowsize=2.5, penwidth=3];\n", *head(list));
    fprintf(graphFilePtr, "tail_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *tail(list), REVERSE_CHAIN_COLOR);
    fprintf(graphFilePtr, "free_head_label -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *freeInd(list), FREE_CHAIN_COLOR);
    
    // установка нодов по индексам
    fprintf(graphFilePtr, "node0 -> node1[style=invis, weight = 100000]");
    fprintf(graphFilePtr, "\t");
    for(listVal_t curCellInd = 1; curCellInd < (listVal_t) list->capacity; curCellInd++){
        fprintf(graphFilePtr, "node%d", curCellInd);
        if(curCellInd != (listVal_t) list->capacity - 1){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[style=invis, weight=1000000];\n");
        }
    }

    /// установка связи между занятыми элементами списка от *head(list)
    fprintf(graphFilePtr, "\t");
    for(listVal_t curCellInd = *head(list); (*prev(list, curCellInd) != *tail(list)) && (*data(list, curCellInd) != LIST_POISON); curCellInd = *next(list, curCellInd)){
        fprintf(graphFilePtr, "node%d", curCellInd);
        if(curCellInd != *tail(list)){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=2, weight=100000];\n", DIRECT_CHAIN_COLOR);
        }
    }

    fprintf(graphFilePtr, "\n\t");
    for(listVal_t curCellInd = *freeInd(list); curCellInd < (listVal_t) list->capacity; curCellInd = *next(list, curCellInd)){
        fprintf(graphFilePtr, "node%d", curCellInd);
        if(*next(list, curCellInd) != (listVal_t) list->capacity){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=2, weight=100000];\n", FREE_CHAIN_COLOR);
        }
    }

    fprintf(graphFilePtr, "\n");
    for(listVal_t curCellInd = *freeInd(list); curCellInd < (listVal_t) list->capacity; curCellInd = *next(list, curCellInd)){
        
        fprintf(graphFilePtr, "\tnode%d[color = \"%s\", penwidth=4];\n", curCellInd, FREE_CHAIN_COLOR);

    }

    for(listVal_t curCellInd = *tail(list); (*next(list, curCellInd) != *head(list)) || (*data(list, curCellInd) != LIST_POISON); curCellInd = *prev(list, curCellInd)){
        fprintf(graphFilePtr, "node%d", curCellInd);
        if(curCellInd != *head(list)){
            fprintf(graphFilePtr, " -> ");
        }
        else{
            fprintf(graphFilePtr, "[color=\"red\", arrowsize=1.5, penwidth=2, weight=100000, headport=n];\n");
        }
    }

    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    
    
    char convertToImageCommandString[CONVERSION_COMMAND_SIZE];

    sprintf(convertToImageCommandString, CONVERT_TO_IMAGE_COMMAND, logCount);   

    system((const char*) convertToImageCommandString);
}