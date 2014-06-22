#ifndef _SET_H_
#define _SET_H_

#include "constant.h"

typedef struct set Set;

struct set{
    void (*add)(Set *s,int);
    int  (*size)(Set *s);
    int  (*visit)(Set *s,int);

    int a[ MAXNUM_STATES ];
    int num;
};

void initSet(Set *s);

#endif
