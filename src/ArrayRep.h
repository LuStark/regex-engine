#ifndef ARRAYREP_INCLUDED
#define ARRAYREP_INCLUDED

#include "Array.h"
#define T Array_T

struct T 
{
    int length;
    int size;
    char *array;
};

extern void ArrayRep_init(T array, int length,
            int size, void *ary);

#undef T
#endif
