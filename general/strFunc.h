#ifndef STR_FUNC_H
#define STR_FUNC_H

#include <stddef.h>

struct string {
    char* ptr;
    size_t len;
};

struct buffer_t{
    int* ptr;
    size_t size;
};

const int UPPER_SYM_MIN        = (int) 'A'; 
const int UPPER_SYM_MAX        = 'Z'; 
const int UPPER_TO_LOWER_SHIFT = 'A' - 'a'; 
const int SIZE_OF_END_FILE     = 1; 

const char END_STR             = '\n';

size_t countStrings(char* buf, size_t fileSize, char endStr);
size_t myStrLen(const char* start, char endStr ='\0');

#endif /* STR_FUNC_H */