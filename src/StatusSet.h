#ifndef STATUS_SET
#define STATUS_SET

#include "Array.h"
#include "NFA.h"
#include "Status.h"

typedef struct StatusSet
{
    Array_T epsClosure;
    bool    hasFinalStatus;
}StatusSet;

typedef struct
{
    StatusSet   H[300];
    int         size;
}StatusSet_table;


/* inverse是状态到集合的倒排索引 */
void get_two_StatusSet_Table (NFA nfa, StatusSet_table*, int inverse[]);

Array_T reach_Status (NFA nfa, StatusSet currSet, int inverse[], wchar_t c);

extern void printStatusSet(StatusSet S);

#endif
