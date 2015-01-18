#ifndef EDGE_INCLUEDED
#define EDGE_INCLUEDED

//#include "Status.h"
//#include "EdgeArray.h"
#include <stdbool.h>
#include <wchar.h>

typedef struct Status Status;
typedef struct Edge Edge;
typedef struct EdgeArray EdgeArray;
typedef struct CharArray *String;

typedef Edge  Condition;

typedef unsigned long long int ULL64;

typedef struct bitvector128 ULL128;

typedef struct {
    wchar_t from, to;
}Range;

#define RangeEqual(r1,r2)\
    (r1.from==r2.from)&&(r1.to==r2.to)


void 
setZero(ULL128 *b);

ULL128
Or (ULL128 b1, ULL128 b2);

ULL128
getMatchBitVector(Edge *e);


extern Edge*    create_edge ();
extern Edge*    create_epsilon_edge();

extern void init_edge(Edge **e);

extern void free_edge(Edge **e);

extern void add_character(Edge **e, wchar_t c);

extern void add_range(Edge **e, Range r);

extern bool is_matched_in_edge(Edge *e, wchar_t c);

extern void setMatchRangeOrNot(Edge **e, int);

extern void
setFromToStatus(Edge **e, Status *from, Status *to);

extern String 
getEdgeContent(Edge *e);

extern void printEdge(Edge*);
extern Edge *CopyEdge(Edge*);
extern void unreferenceStatus(Edge**); 

extern int size_of_edge();
extern Status* get_from_status (Edge *e);
extern Status* get_to_status (Edge *e);

extern void set_epsilon (Edge **e);

extern void output_edge_crossTable (Edge *e);
extern void copyEdge_without_Status (Edge *to, Edge *from);
extern bool isEpsilon(Edge *e);


#endif
