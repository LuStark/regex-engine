#ifndef EDGE_INCLUEDED
#define EDGE_INCLUEDED

#include "typedef.h"
#include <stdbool.h>

typedef struct Edge *Edge;
#define E Edge
typedef E   Condition;

typedef unsigned long long int ULL64;

typedef struct bitvector128 {
    ULL64   L64;
    ULL64   H64;
}ULL128;


void 
setZero(ULL128 *b);

ULL128
Or (ULL128 b1, ULL128 b2);

ULL128
getMatchBitVector(E e);


#include "Status.h"


extern E        allocEdge ();
extern E        allocEpsilonEdge();
extern Array_T  allocEdgeArray(int);
extern void
freeEdge (E e);

extern void addCharacter (E e, wchar_t c);
extern void addRange (E e, Range r);
extern int  crossEdge (E e, wchar_t c);
extern void setMatchRangeOrNot (E e, int);

extern void
setFromToStatus (E e, Status from, Status to);

extern Array_T  
getEdgeContent(E e);

extern void printEdge (E);
extern E    CopyEdge (E);
extern void unreferenceStatus (E); 
extern void clearBits(E e);

extern int sizeOfEdge();
extern Status getfromStatus (E e);
extern Status gettoStatus (E e);

extern void setEpsilon(E e);
extern void outputEdgeCrossTable (E e);
extern void copyEdge_without_Status (E to, E from);
extern bool isEpsilon(E e);

#undef E

#endif
