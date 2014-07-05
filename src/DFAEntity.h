#ifndef _DFAENTITY_H_
#define _DFAENTITY_H_

#include "constant.h"
#include "NFA.h"

typedef int     **DFATable;

typedef struct dfa_entity {
    DFATable    T;
    DFA         *dfa;
    char        *name;
    int         numOfStatus;
} DFAEntity;

DFATable
makeUpDFATable( DFA *dfa );

bool
Recognition( DFAEntity entity, xchar_t *normal );

#endif
