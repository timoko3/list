#ifndef PROTECTION_H
#define PROTECTION_H

#include "general_list.h"

#include <stdio.h>

listStatus verifyList(list_t* list, const char* function, const char* file, const int line);

void htmlLog(list_t* list, const char* callFileName, const char* callFuncName, int callLine,
                           const char* text, ...);
void listDumpBasic(list_t* list, FILE* stream);
void listGraphDump(list_t* list);

#endif /* PROTECTION_H */