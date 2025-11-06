#ifndef PROTECTION_H
#define PROTECTION_H

#include "general_classical_list.h"

#include <stdio.h>

listClassicalStatus verifyList(listClassical_t* list, const char* function, const char* file, const int line);

void htmlLog(listClassical_t* list, const char* callFileName, const char* callFuncName, int callLine,
                           const char* callCase, const char* actionName, long long parameter);
void listDumpBasic(listClassical_t* list, FILE* stream);
void listGraphDump(listClassical_t* list);

#endif /* PROTECTION_H */