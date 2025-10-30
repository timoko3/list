#ifndef PROTECTION_H
#define PROTECTION_H

#include "general_list.h"

#include <stdio.h>

void htmlLog(list_t* list, const char* callFileName, const char* callFuncName, int callLine);
void listDumpBasic(list_t* list, FILE* stream);
void listGraphDump(list_t* list);

#endif /* PROTECTION_H */