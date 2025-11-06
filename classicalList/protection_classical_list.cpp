#include "protection_classical_list.h"
#include "../general/file.h"
#include "../general/debug.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
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
        else if(!connectivityCheck(list)){
            assignErrorStruct(list, LIST_NOT_CONNECTED);
        }

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

static bool connectivityCheck(list_t* list){
    assert(list);

    size_t connectionsCount = 0;
    for(listElem_t* curCell = *head(list); *next(list, curCell) != list->dummy; curCell = *next(list, curCell)){
        connectionsCount++;
    }
    size_t referenceConnectionsCount = 0;

    if(list->size != 0){
        referenceConnectionsCount = list->size - 1;
    }

    fprintf(stdout, "connectionsCount: %lu, size: %lu\n", connectionsCount, referenceConnectionsCount);
    if(connectionsCount != referenceConnectionsCount){
        return false;
    }
    return true;
}

void htmlLog(list_t* list, const char* callFileName, const char* callFuncName, int callLine,
             const char* callCase, const char* actionName, long long parameter){
    assert(list);
    assert(callFileName);
    assert(callFuncName);
    assert(callCase);
    assert(actionName);

    fileDescription logFile = {};
    if (logCount == 0) {
        logFile = (fileDescription){ HTML_LOG_FILE_NAME, "wb" };
    } 
    else {
        logFile = (fileDescription){ HTML_LOG_FILE_NAME, "ab" };
    }

    FILE* logFilePtr = myOpenFile(&logFile);
    assert(logFilePtr);

    if (logCount == 0) {
        fprintf(logFilePtr,
            "<!DOCTYPE html><html lang=\"ru\"><head>"
            "<meta charset=\"utf-8\">"
            "<title>Classical List Dump</title>"
            "<style>"
              "body{font-family:'Segoe UI',Tahoma,sans-serif;background:#e6ecf2;color:#1e272e;"
                  "margin:20px;font-size:18px;line-height:1.6;}"
              "h2,h3,h4{margin:8px 0 6px 0;}"
              ".dump-card{background:#ffffff;border-radius:14px;padding:22px 26px;"
                  "box-shadow:0 6px 22px rgba(0,0,0,0.12);margin-bottom:36px;}"
              ".dump-header{display:flex;flex-wrap:wrap;align-items:center;justify-content:space-between;"
                  "gap:16px;margin-bottom:10px;}"
              ".case-badge{font-weight:700;color:#fff;padding:8px 14px;border-radius:8px;font-size:1.2rem;}"
              ".case-badge.error{background:#ff5252;} .case-badge.info{background:#007bff;} .case-badge.ok{background:#00b894;}"
              ".location{background:#fff2cc;border-left:8px solid #ffb300;padding:12px 16px;border-radius:10px;"
                  "font-size:1.05rem;line-height:1.4;}"
              "table{border-collapse:collapse;width:100%%;font-family:monospace;margin-top:14px;font-size:1rem;}"
              "th,td{border:1px solid #d0d7de;padding:10px 14px;text-align:center;}"
              "th{background:#f0f4fa;font-weight:700;}"
              ".used{color:#006400;font-weight:700;}"
              ".free{background:#ffeef0;color:#9c2c3c;}"
              ".graph-wrap{text-align:center;margin-top:18px;}"
              ".graph-link{display:inline-block;border-radius:10px;overflow:hidden;"
                  "box-shadow:0 8px 32px rgba(0,0,0,0.18);}"
              ".small-meta{color:#444;font-size:0.95rem;margin-top:8px;}"
            "</style></head><body>\n");
    }

    fprintf(logFilePtr, "<div class=\"dump-card\">\n");

    const char* caseClass = "info";
    if (list->status.type != PROCESS_OK) caseClass = "error";

    fprintf(logFilePtr,
        "<div class=\"dump-header\">"
          "<div>"
            "<h2 style=\"margin:0; font-size:1.8rem;\"> DUMP — "
            "<span class=\"case-badge %s\">%s</span> "
            "<span style=\"margin-left:10px; color:#333;\">%s(0x%12llx)</span>"
            "</h2>"
          "</div>"
          "<div class=\"location\">"
            "<b>File:</b> %s<br>"
            "<b>Function:</b> %s<br>"
            "<b>Line:</b> %d"
          "</div>"
        "</div>\n",
        caseClass, callCase, actionName, (unsigned long long)parameter,
        callFileName, callFuncName, callLine);

    if (parameter == -1) {
        fprintf(logFilePtr,
            "<p style=\"color:#b00020;font-weight:700;margin-top:12px;font-size:1.1rem;\">%s</p>\n",
            list->status.text);
    }

    fprintf(logFilePtr,
        "<h4 style=\"margin-top:16px;margin-bottom:8px;font-size:1.3rem;\"> Переменные списка</h4>\n"
        "<table><tbody>"
        "<tr><td><b>dummyAddr</b></td><td>%p</td></tr>"
        "<tr><td><b>size</b></td><td>%lu</td></tr>"
        "<tr><td><b>head</b></td><td>%p</td></tr>"
        "<tr><td><b>tail</b></td><td>%p</td></tr>"
        "</tbody></table>\n",
        (void*)list->dummy, list->size, *head(list), *tail(list));

    fprintf(logFilePtr,
        "<h4 style=\"margin-top:18px;margin-bottom:8px;font-size:1.3rem;\"> Элементы списка</h4>\n"
        "<table><thead><tr>"
        "<th>Addr</th><th>Data</th><th>Prev</th><th>Next</th>"
        "</tr></thead><tbody>\n");

    for (listElem_t* curCell = *head(list);
         *data(list, curCell) != LIST_POISON;
         curCell = *next(list, curCell)){
        bool isFree = (*data(list, curCell) == LIST_POISON);
        fprintf(logFilePtr, "<tr class=\"%s\">", isFree ? "free" : "");
        fprintf(logFilePtr, "<td>%p</td>", curCell);

        if (isFree){
            fprintf(logFilePtr, "<td>PZN</td>");
        }
        else{
            fprintf(logFilePtr, "<td class=\"used\">%d</td>", *data(list, curCell));
        }

        fprintf(logFilePtr, "<td>%p</td><td>%p</td></tr>\n",
                *prev(list, curCell), *next(list, curCell));
    }

    fprintf(logFilePtr, "</tbody></table>\n");

    listGraphDump(list);  

    
    double widthPercent = (double)SCALE_KOEF + (double)list->size * (double)SCALE_KOEF;
    if(logCount == 1){
        widthPercent = 40;
    }

    fprintf(logFilePtr,
        "<h4 style=\"margin-top:20px;margin-bottom:10px;font-size:1.3rem;\"> Визуализация списка</h4>\n"
        "<div class=\"graph-wrap\">"
          "<a class=\"graph-link\" href=\"graphDumps/graph%lu.png\" target=\"_blank\" title=\"Открыть в полном размере\">"
            "<img src=\"graphDumps/graph%lu.png\" style=\"width:%0.3f%%;height:auto;display:block;\" alt=\"graph dump\">"
          "</a>"
          "<div class=\"small-meta\">Масштаб: width = %0.3f%%</div>"
        "</div>\n",
        logCount, logCount, widthPercent, widthPercent);

    fprintf(logFilePtr, "</div>\n");
    fclose(logFilePtr);
}


void listDumpBasic(list_t* list, FILE* stream){
    assert(list);
    assert(stream);

    fprintf(stream, "<h3 class=\"section-title\">dump (raw)</h3>\n");
    fprintf(stream, "<pre>\n");
    fprintf(stream, "dummyAddr: %p\n", list->dummy);
    fprintf(stream, "size: %zu\n", list->size);
    fprintf(stream, "head: %p\n", *head(list));
    fprintf(stream, "tail: %p\n", *tail(list));
    fprintf(stream, "elements:\n");
    for (listElem_t* curCell = *head(list); *data(list, curCell) != LIST_POISON; curCell = *next(list, curCell)){
        fprintf(stream, "\taddr: %p, data: %-3d, next: %p, prev: %p\n",
                curCell,
                *data(list, curCell),
                *next(list, curCell),
                *prev(list, curCell));
    }
    fprintf(stream, "</pre>\n");
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
    

    fprintf(graphFilePtr, "node%d [label=\"address = %p | data = PZN | {tail = %p | head = %p} \", shape=record, style=\"filled\", fillcolor=\"#222222\", fontcolor=\"yellow\", color=\"yellow\", penwidth=2];\n", (listVal_t)(uintptr_t) list->dummy, list->dummy, *tail(list), *head(list));
    
    for(listElem_t* curCell = *head(list); *data(list, curCell) != LIST_POISON; curCell = *next(list, curCell)){
        fprintf(graphFilePtr, "\tnode%d [label=\"address = %p | data = %d | {prev = %p | next = %p} \"];\n",(listVal_t)(uintptr_t) curCell, curCell, *data(list, curCell), *prev(list, curCell), *next(list, curCell));
    }
    fprintf(graphFilePtr, "\n"); 
    
    fprintf(graphFilePtr, "head_label      [shape=box, width = 2.4, height = 1.4, label=\"HEAD\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      [shape=box, width = 2.4, height = 1.4, label=\"TAIL\", style=\"filled\", fillcolor=\"#BBDDEE\", color=\"%s\", penwidth = 6,  fontcolor=\"darkblue\", fontsize = 40];\n", REVERSE_CHAIN_COLOR);


    fprintf(graphFilePtr, "head_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", (listVal_t)(uintptr_t) *head(list),    DIRECT_CHAIN_COLOR);
    fprintf(graphFilePtr, "tail_label      -> node%d [color=\"%s\"  , arrowsize=2.5, penwidth=3];\n", (listVal_t)(uintptr_t) *tail(list),    REVERSE_CHAIN_COLOR);
    $
    // установка нодов по индексам
    fprintf(graphFilePtr, "\t");
    fprintf(graphFilePtr, "node%d -> node%d[style=invis, weight = 100000];\n", (listVal_t)(uintptr_t) list->dummy, (listVal_t)(uintptr_t) *head(list));
    for(listElem_t* curCell = *head(list); *data(list, curCell) != LIST_POISON && list->size > 1; curCell = *next(list, curCell)){
        fprintf(graphFilePtr, "node%d", (listVal_t)(uintptr_t) curCell);
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

    // printf("head: %p, tail: %p\n", *head(list), *tail(list));

    bool startPass = true;
    for(listElem_t* curCell = *head(list); (curCell != *head(list)) || startPass; curCell = *next(list, curCell)){
        startPass = false;

        if(curCell != list->dummy) fprintf(graphFilePtr, "node%d [fillcolor = \"%s:%s\", fontcolor = \"%s\"]\n", (listVal_t)(uintptr_t) curCell, DIRECT_CHAIN_COLOR , REVERSE_CHAIN_COLOR, BORDER_CHAIN_COLOR);
        else                       fprintf(graphFilePtr, "node%d \n", (listVal_t)(uintptr_t) curCell);

        fprintf(graphFilePtr, "node%d", (listVal_t)(uintptr_t) curCell);
        
        fprintf(graphFilePtr, " -> ");

        fprintf(graphFilePtr, "node%d", (listVal_t)(uintptr_t) *next(list, curCell));

        fprintf(graphFilePtr, "[color=\"%s:%s\", arrowsize=1.5, penwidth=5, weight=1000, constraint=false, dir = both];\n", DIRECT_CHAIN_COLOR, REVERSE_CHAIN_COLOR);
    }

    fprintf(graphFilePtr, "\n}");

    fclose(graphFilePtr);
    
    char convertToImageCommandString[CONVERSION_COMMAND_SIZE];

    sprintf(convertToImageCommandString, CONVERT_TO_IMAGE_COMMAND, logCount);   

    system((const char*) convertToImageCommandString);
}