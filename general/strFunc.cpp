#include "strFunc.h"

#include <assert.h>

size_t myStrLen(const char* start, char endStr){
    assert(start);

    size_t curSymInd = 0;
    while(start[curSymInd] != endStr) curSymInd++;
    
    return curSymInd;
}

size_t countStrings(char* buf, size_t fileSize, char endStr){
    assert(buf);

    size_t nStrings = 1;

    size_t curSym = 0;
    while(curSym < fileSize){
        if(buf[curSym] == endStr){
            nStrings++;
        }
        curSym++;
    }

    return nStrings;
}