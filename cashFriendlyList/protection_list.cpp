#include "protection_list.h"
#include "../general/file.h"
#include "string.h"

#define DEBUG
#include "../general/debug.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

static const char* GRAPH_DUMP_DOT_FILE_NAME = "graphDumps/graphDump.dot";
static const char* LOG_FILE_NAME            = "logList.html";
static const listVal_t MAX_NODE_NAME_SIZE   = 20;
static const char* CONVERT_TO_IMAGE_COMMAND = "dot -Tpng graphDumps/graphDump.dot -o graphDumps/graph%d.png";
static const size_t CONVERSION_COMMAND_SIZE = 61;

static const char* DIRECT_CHAIN_COLOR  = "#6A5ACD";
static const char* REVERSE_CHAIN_COLOR = "#c75a96ff";
static const char* BORDER_CHAIN_COLOR  = "#ffe9b5ff";
static const char* FREE_CHAIN_COLOR    = "#326932";
static const char* FREE_NODE_FILLCOLOR = "#16c56eff";
static const char* FREE_NODE_FONTCOLOR = "#222926ff";


static size_t logCount = 0;
static double SCALE_KOEF = 8.5;

static void assignErrorStruct(list_t* list, listStatus type);

static bool connectivityCheck(list_t* list);
static bool indexValidityCheck(list_t* list);

listStatus verifyList(list_t* list, const char* function, const char* file, const int line){
    assert(function);
    assert(file);

    if(list == NULL){
        LPRINTF("list — нулевой указатель\n");  
    } 
    else{
        if(list->elem == NULL){  
            assignErrorStruct(list, NULL_POINTER);
            LPRINTF("data — нулевой указатель\n");
        }
        else if(list->capacity == 0){
            assignErrorStruct(list, CAPACITY_IS_ZERO);
        }
        else if(list->size > list->capacity){
            assignErrorStruct(list, SIZE_EXCEEDS_CAPACITY);
        }
        else if(!indexValidityCheck(list)){
            assignErrorStruct(list, NON_VALID_INDEXES);
        }
        else if(malloc_usable_size(list->elem) != (sizeof(listElem_t) * list->capacity)){
            assignErrorStruct(list, BAD_MEMORY_ALLOCATION);
        }

        else if(!connectivityCheck(list)){
            assignErrorStruct(list, LIST_NOT_CONNECTED);
        }

        else{
            assignErrorStruct(list, PROCESS_OK);
            return PROCESS_OK;
        }
    }

    LPRINTF("%s\n", list->status.text);
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

static bool indexValidityCheck(list_t* list){
    assert(list);
    $
    for(size_t curCellInd = 0; curCellInd < list->capacity; curCellInd++){
        if(*next(list, (listVal_t) curCellInd) > (listVal_t) list->capacity || *prev(list, (listVal_t) curCellInd) > (listVal_t) list->capacity){
            return false;
        }
    }

    return true;
}

static bool connectivityCheck(list_t* list){
    assert(list);

    size_t connectionsCount = 0;
    for(listVal_t curCellInd = *head(list); (curCellInd != *tail(list)); curCellInd = *next(list, curCellInd)){
        connectionsCount++;
    }
    size_t referenceConnectionsCount = 0;

    if(list->size != 0){
        referenceConnectionsCount = list->size - 1;
    }

    LPRINTF("connectionsCount: %lu, size: %lu\n", connectionsCount, referenceConnectionsCount);
    if(connectionsCount != referenceConnectionsCount){
        return false;
    }
    return true;
}

void htmlLog(list_t* list, const char* callFileName, const char* callFuncName, int callLine,
             const char* dumpDescription, ...){
    assert(list);
    assert(callFileName);
    assert(callFuncName);
    assert(dumpDescription);

    fileDescription logFile = {};
    if (logCount == 0) {
        logFile = (fileDescription){ LOG_FILE_NAME, "wb" };
    } 
    else {
        logFile = (fileDescription){ LOG_FILE_NAME, "ab" };
    }

    FILE* logFilePtr = myOpenFile(&logFile);
    assert(logFilePtr);

    /* Заголовок и стили */
    if (logCount == 0) {
        fprintf(logFilePtr,
        "<style>"
        "body{font-family:'Segoe UI',Tahoma,monospace;background:#f4f6f9;color:#111;margin:6px 12px;font-size:12px;line-height:1.1;}"
        "h2,h3,h4{margin:3px 0 2px 0;font-size:14px;}"
        ".dump-card{background:#fff;border-radius:6px;padding:4px 6px;box-shadow:0 1px 4px rgba(0,0,0,0.08);margin-bottom:10px;}"
        ".dump-header{display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;gap:4px;margin-bottom:4px;}"
        ".case-badge{font-weight:700;color:#fff;padding:2px 6px;border-radius:4px;font-size:0.8rem;}"
        ".case-badge.error{background:#ff5252;} .case-badge.info{background:#007bff;} .case-badge.ok{background:#00b894;}"
        ".location{background:#fff2cc;border-left:4px solid #ffb300;padding:4px 6px;border-radius:4px;font-size:0.75rem;line-height:1.1;}"
        "table{border-collapse:collapse;width:100%%;font-family:monospace;margin-top:3px;font-size:0.8rem;table-layout:fixed;}"
        "th,td{border:0.5px solid #bbb;padding:1px 3px;text-align:center;vertical-align:middle;}"
        "th{background:#eef2f5;font-weight:600;}"
        "td{height:14px;}"
        ".used{color:#003c00;font-weight:600;}"
        ".free{background:#fff4f4;color:#a33;}"
        ".graph-wrap{text-align:center;margin-top:6px;}"
        ".graph-link{display:inline-block;border-radius:4px;overflow:hidden;box-shadow:0 2px 8px rgba(0,0,0,0.1);}"
        ".small-meta{color:#555;font-size:0.7rem;margin-top:2px;}"
        "</style></head><body>\n");
    }

    fprintf(logFilePtr, "<div class=\"dump-card\">\n");

    const char* caseClass = "info";
    if (list->status.type != PROCESS_OK) caseClass = "error";

    fprintf(logFilePtr, "<div class=\"dump-header\">"
                        "<div>"
                        "<h2 style=\"margin:0; font-size:1.8rem;\"> DUMP — <span style=\"margin-left:10px; color:#333;\"> ");

    va_list args;
    va_start(args, dumpDescription);
    vfprintf(logFilePtr,  dumpDescription, args);
    va_end(args);

    fprintf(logFilePtr, "</span>"
                        "</h2>"
                        "</div>");

    fprintf(logFilePtr,          
            "<div class=\"location\">"
            "<b>File:</b> %s<br>"
            "<b>Function:</b> %s<br>"
            "<b>Line:</b> %d"
          "</div>"
        "</div>\n", callFileName, callFuncName, callLine);
    
    /* Переменные списка */
    fprintf(logFilePtr,
        "<h4 style=\"margin-top:16px;margin-bottom:8px;font-size:1.3rem;\"> Переменные списка</h4>\n"
        "<table><tbody>"
        "<tr><td><b>capacity</b></td><td>%lu</td></tr>"
        "<tr><td><b>size</b></td><td>%lu</td></tr>"
        "<tr><td><b>head</b></td><td>%d</td></tr>"
        "<tr><td><b>tail</b></td><td>%d</td></tr>"
        "<tr><td><b>free index</b></td><td>%d</td></tr>"
        "</tbody></table>\n",
        list->capacity, list->size, *head(list), *tail(list), *freeInd(list));

    /* Таблица элементов */
    fprintf(logFilePtr,
        "<h4 style=\"margin-top:18px;margin-bottom:8px;font-size:1.3rem;\"> Элементы списка</h4>\n"
        "<table><thead><tr>"
        "<th>Index</th><th>Data</th><th>Prev</th><th>Next</th>"
        "</tr></thead><tbody>\n");

    for (listVal_t i = 0; i < (listVal_t)list->capacity; i++) {
        bool isFree = (*data(list, i) == LIST_POISON);
        fprintf(logFilePtr, "<tr class=\"%s\">", isFree ? "free" : "");
        fprintf(logFilePtr, "<td>%d</td>", i);
        if (isFree)
            fprintf(logFilePtr, "<td>PZN</td>");
        else
            fprintf(logFilePtr, "<td class=\"used\">%d</td>", *data(list, i));
        fprintf(logFilePtr, "<td>%d</td><td>%d</td></tr>\n", *prev(list, i), *next(list, i));
    }

    fprintf(logFilePtr, "</tbody></table>\n");

    /* Генерация графа */
    listGraphDump(list);

    double widthPercent = (double)list->capacity * SCALE_KOEF;

    fprintf(logFilePtr,
        "<h4 style=\"margin-top:20px;margin-bottom:10px;font-size:1.3rem;\"> Визуализация списка</h4>\n"
        "<div class=\"graph-wrap\">"
          "<a class=\"graph-link\" href=\"graphDumps/graph%lu.png\" target=\"_blank\" title=\"Открыть в полном размере\">"
            "<img src=\"graphDumps/graph%lu.png\" style=\"width:%0.3f%%;height:auto;display:block;\" alt=\"graph dump\">"
          "</a>"
          "<div class=\"small-meta\">Клик чтобы увидить картинку полностью</div>"
        "</div>\n",
        logCount, logCount, widthPercent);

    fprintf(logFilePtr, "</div>\n");

    fclose(logFilePtr);
}



void listDumpBasic(list_t* list, FILE* stream){
    assert(list);
    assert(stream);

    fprintf(stream, "listDump:\n");

    fprintf(stream, "\tcapacity: %lu\n", list->capacity);
    fprintf(stream, "\tsize: %lu\n",     list->size);
    

    fprintf(stream, "\thead: %d\n",        *head(list));
    fprintf(stream, "\ttail: %d\n",        *tail(list));
    fprintf(stream, "\tcurFreeElem: %d\n", *freeInd(list));    

    fprintf(stream, "\telements:\n");
    for(listVal_t curElemInd = 0; curElemInd < (listVal_t) list->capacity; curElemInd++){
        if(*data(list, (listVal_t) curElemInd) != LIST_POISON){
            fprintf(stream, "\t\tdata: %-10d, next: %-3d, prev: %-3d\n", *data(list, (listVal_t) curElemInd), 
                                                                         *next(list, (listVal_t) curElemInd), 
                                                                         *prev(list, (listVal_t) curElemInd));
        }                                                                         
        else{
            fprintf(stream, "\t\tdata: PZN, next: %-3d, prev: %-3d\n", *next(list, (listVal_t) curElemInd), 
                                                                       *prev(list, (listVal_t) curElemInd));
        }
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

    fprintf(graphFilePtr, "nodesep = 1;\n");
    fprintf(graphFilePtr, "ranksep = 1\n");
    
    fprintf(graphFilePtr, "\tnode [shape=record, style=\"filled\", fillcolor=\"#FFA089\", fontcolor=\"black\", color=\"#007CAD\", penwidth=2.5, fontname=\"Tahoma\", fontsize=25];\n\n");
    fprintf(graphFilePtr, "edge [color=\"#2d714f\", arrowsize=1, penwidth=5, arrowhead=\"vee\", style=\"bold\"];\n");
    

    size_t nodesAmount = 0;
    for(listVal_t curCellInd = 0; curCellInd < (listVal_t) list->capacity; curCellInd++){
        if(curCellInd == 0){
            fprintf(graphFilePtr, "node0 [label=\"phys ind = 0 | head = %d | tail = %d | free_tail = %d\", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", *head(list), *tail(list), *freeInd(list));
            continue;
        }
        
        if(*data(list, curCellInd) != LIST_POISON){
            fprintf(graphFilePtr, "\tnode%d [label=\"phys idx = %d | data = %d | {prev = %d | next = %d} \"];\n", curCellInd, curCellInd, *data(list, curCellInd), *prev(list, curCellInd), *next(list, curCellInd));
        }
        else{
            fprintf(graphFilePtr, "\tnode%d [label=\"phys idx = %d | data = PZN | {prev = %d | next = %d} \"];\n", curCellInd, curCellInd, *prev(list, curCellInd), *next(list, curCellInd));
        }
        
        nodesAmount++;
    }
    fprintf(graphFilePtr, "\n"); 
    
    fprintf(graphFilePtr, "head_label      [shape=box, width = 2.4, height = 1.4, label=\"HEAD\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      [shape=box, width = 2.4, height = 1.4, label=\"TAIL\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", REVERSE_CHAIN_COLOR);
    fprintf(graphFilePtr, "free_head_label [shape=box, width = 2.4, height = 1.4, label=\"FREE\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", FREE_CHAIN_COLOR);


    fprintf(graphFilePtr, "head_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *head(list),    DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *tail(list),    REVERSE_CHAIN_COLOR);
    fprintf(graphFilePtr, "free_head_label -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", *freeInd(list), FREE_CHAIN_COLOR);
    $
    // установка нодов по индексам
    fprintf(graphFilePtr, "node0 -> node1[style=invis, weight = 100000];\n");
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

    fprintf(graphFilePtr, "\t");
    bool isPrevNodeBad = false;
    for(listVal_t curCellInd = 0; curCellInd < (listVal_t) list->capacity; curCellInd++){
        if(*head(list) == 0 && *tail(list) == 0 && curCellInd == 0){
            continue;
        }

        if(*next(list, curCellInd) == 0){
            fprintf(graphFilePtr, "node%d [fillcolor = \"%s:%s\", fontcolor = \"%s\"]\n", curCellInd,              DIRECT_CHAIN_COLOR , REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);

            fprintf(graphFilePtr, "node%d", curCellInd);
            
            fprintf(graphFilePtr, " -> ");

            fprintf(graphFilePtr, "node%d", *next(list, curCellInd));
            fprintf(graphFilePtr, "[color=\"%s:%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false, dir = both];\n", DIRECT_CHAIN_COLOR, REVERSE_CHAIN_COLOR);

            continue;
        }

        
        if(*data(list, curCellInd) == LIST_POISON && curCellInd != 0){
            fprintf(graphFilePtr, "node%d [fillcolor = \"%s\", fontcolor = \"%s\"]\n", curCellInd,                FREE_NODE_FILLCOLOR, FREE_NODE_FONTCOLOR);

            if(curCellInd < (listVal_t) list->capacity - 1){
                fprintf(graphFilePtr, "node%d [fillcolor = \"%s\", fontcolor = \"%s\"]\n", *next(list, curCellInd),   FREE_NODE_FILLCOLOR, FREE_NODE_FONTCOLOR);
            }
        }
        else if(curCellInd != 0){
            
            if(*next(list, curCellInd) > (listVal_t) list->capacity){
                fprintf(graphFilePtr, "node%d [fillcolor = \"%s\", fontcolor = \"%s\"]\n", curCellInd,              DIRECT_CHAIN_COLOR, BORDER_CHAIN_COLOR);
            }
            else{
                fprintf(graphFilePtr, "node%d [fillcolor = \"%s:%s\", fontcolor = \"%s\"]\n", curCellInd,              DIRECT_CHAIN_COLOR , REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);
                if(curCellInd < (listVal_t) list->capacity - 1){
                    fprintf(graphFilePtr, "node%d [fillcolor = \"%s:%s\", fontcolor = \"%s\"]\n", *next(list, curCellInd), DIRECT_CHAIN_COLOR, REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);
                }
            }
        }

        if(isPrevNodeBad){
            fprintf(graphFilePtr, "node%d [fillcolor = \"%s\", fontcolor = \"%s\"]\n", curCellInd,              REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);
        }
        
        if(curCellInd < (listVal_t) list->capacity - 1){
            fprintf(graphFilePtr, "node%d", curCellInd);
            
            fprintf(graphFilePtr, " -> ");

            fprintf(graphFilePtr, "node%d", *next(list, curCellInd));

            if(*data(list, curCellInd) == LIST_POISON && curCellInd != 0){
                fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false, tailport = n];\n", FREE_CHAIN_COLOR);
            }
            else{
                if(*next(list, curCellInd) > (listVal_t) list->capacity){
                    fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false];\n", DIRECT_CHAIN_COLOR);
                }
                else{
                    fprintf(graphFilePtr, "[color=\"%s:%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false, dir = both];\n", DIRECT_CHAIN_COLOR, REVERSE_CHAIN_COLOR);
                }


                if(isPrevNodeBad){
                    fprintf(graphFilePtr, "node%d", curCellInd);
            
                    fprintf(graphFilePtr, " -> ");

                    fprintf(graphFilePtr, "node%d", *prev(list, curCellInd));

                    fprintf(graphFilePtr, "[color=\"%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false];\n", REVERSE_CHAIN_COLOR);

                    isPrevNodeBad = false;
                }

                if(*next(list, curCellInd) > (listVal_t) list->capacity) isPrevNodeBad = true;
            }
        }
    }

    if(list->status.type == NON_VALID_INDEXES){
        for(size_t curCellInd = 0; curCellInd < list->capacity; curCellInd++){
            if(*next(list, (listVal_t) curCellInd) > (listVal_t) list->capacity){
                fprintf(graphFilePtr, "\tnode%d [label=\"phys   Ind = %d\","
                     "shape=doubleoctagon, fillcolor = \"red\", fontcolor=\"white\", color=\"#007CAD\", penwidth=3, fontname=\"Tahoma Bold\", fontsize=40];\n",
                     *next(list, (listVal_t) curCellInd), *next(list, (listVal_t) curCellInd));
            }
        }
    }
    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    
    char convertToImageCommandString[CONVERSION_COMMAND_SIZE];
    sprintf(convertToImageCommandString, CONVERT_TO_IMAGE_COMMAND, logCount);   
    system((const char*) convertToImageCommandString);
}