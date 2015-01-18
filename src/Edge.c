#include "Edge.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>


typedef unsigned long long int ULL64;

typedef struct bitvector128 
{
    ULL64   L64;
    ULL64   H64;
}ULL128;


void outputBinary (ULL64 a)
{
    ULL64 i = 1ULL<<63;
    while (i>0)
    {
        wprintf(L"%d", ((i&a)!=0));
        i >>= 1;
    }
}

void setZero(ULL128 *b)
{
    b->L64 = 0L;
    b->H64 = 0L;
}


ULL128
Or (ULL128 b1, ULL128 b2)
{
    ULL128 bb;
    bb.L64 = b1.L64 | b2.L64;
    bb.H64 = b1.H64 | b2.H64;

    return bb;
}


typedef struct Edge Edge; 

struct Edge
{
    ULL128      crossTable;
    bool        is_match;
    Status      *from;
    Status      *to;

    bool        hasEpsilon;
    String      content;
};



Edge*   create_edge ()
{
    Edge *e = malloc(sizeof(struct Edge));
    assert (e);
    
    e -> crossTable.L64 = 0L;
    e -> crossTable.H64 = 0L;
    e -> is_match   = true;
    e -> content    = create_string(L""); 
    return e;
}

Edge*   create_epsilon_edge ()
{
    Edge* e = create_edge ();
    e -> hasEpsilon = true;

    return e;
}


void init_edge(Edge **e)
{
    assert(*e);
    setZero(&((*e)->crossTable));
    (*e) -> from = NULL;
    (*e) -> to = NULL;
    (*e) -> content = create_string(L""); 
    (*e) -> hasEpsilon = false;
    (*e) -> is_match = false;
}

void free_edge(Edge **e)
{
    assert(*e);
    if ((*e) -> content != NULL)
        free_string(&((*e) -> content));
    
    (*e) -> from = NULL;
    (*e) -> to   = NULL;

    (*e) -> crossTable.L64 = 0L;
    (*e) -> crossTable.H64 = 0L;
    (*e) -> is_match = 1;
    (*e) -> hasEpsilon = false;

}

void add_character (Edge **e, wchar_t c)
{
    assert(*e);
    if (c >= 128)
    {
        push_back( &((*e) -> content), c);
        wprintf(L"暂时不处理非ASCII码");
    }

    if (c >= 64 )
        (*e) -> crossTable.H64 |= (1ULL << (c-64));
    else
        (*e) -> crossTable.L64 |= (1ULL << c);

    push_back( &((*e) -> content), c);
}

void add_range(Edge **e, Range r)
{
    wchar_t c1, c2, c;
    c1 = r.from; c2 = r.to;
    if (c2 < c1)
        return;
    int i;

    if (c1 < 64 && c2 >= 64)
    {
        (*e) -> crossTable.L64 = (~(1ULL << c1) + 1);
        (*e) -> crossTable.H64 = (1ULL << (c2 - 64) - 1);
        (*e) -> crossTable.H64 |= (1ULL << (c2 - 64));
    }
    else
    {
        if (c1 >= 64)
        {
            c1 -= 64;
            c2 -= 64;
            (*e) -> crossTable.H64 |= (1ULL<<c2)-(1ULL<<c1);
            (*e) -> crossTable.H64 |= (1ULL<<c2);
        }
        else
        {
            (*e) -> crossTable.L64 |= (1ULL<<c2)-(1ULL<<c1);
            (*e) -> crossTable.L64 |= (1ULL<<c2);
        }
    }
    for (c = c1; c <= c2; c++)
      push_back( &((*e)->content), c);
}

bool is_matched_in_edge (Edge *e, wchar_t c)
{
    int i;

    if (c < 64)
        return (e -> crossTable.L64 & (1ULL << c)) > 0;
    else if (c < 128)
        return (e->crossTable.H64 & (1ULL<<c-64)) > 0;
    else
    {
        for(i = 0; i < get_string_length(e -> content); i++)
        {
            if (c == getChar(e -> content, i))
                return true;
        }
    }
    return false;
}

void setMatchRangeOrNot (Edge **e, int c)
{
    (*e) -> is_match = c;
}

void setFromToStatus(Edge **e, Status *from, Status *to)
{
    assert(*e);

    (*e) -> from = from;
    (*e) -> to = to;
}


String getEdgeContent(Edge *e)
{
    assert(e->content);
    return e->content;
}

void printEdge (Edge *e)
{
    if (!e->is_match)
    {
        wprintf(L"~");
    }
    if (e->hasEpsilon)
    {
        wprintf(L"ε");
        return;
    }
    ULL64   a = 1ULL;
    int     i = 0;
    while (i<64)
    {
        if ((a & e->crossTable.L64) != 0)
          putwchar(i); 
        a <<= 1;
        i++;
    }
    i = 0;
    a = 1ULL;
    while (i<64)
    {
        if ((a & e->crossTable.H64) != 0)
        {
            putwchar(i+64); 
        }
        a <<= 1;
        i++;
    }
}

void copyEdge_without_Status (Edge* to, Edge* from)
{
    to -> crossTable = from -> crossTable;
    to -> is_match   = from -> is_match;
    to -> hasEpsilon = from -> hasEpsilon;
}

void unreferenceStatus (Edge **e)
{
    (*e) -> from = NULL;
    (*e) -> to   = NULL;
}

int  size_of_edge()
{
    return sizeof(struct Edge);
}

Status *get_from_status(Edge *e)
{
    assert (e);
    return e -> from;
}

Status *get_to_status(Edge *e)
{
    assert(e);
    return e->to;
}

void set_epsilon(Edge **e)
{
    assert(*e);
    (*e) -> hasEpsilon = true;
}

void output_edge_crossTable (Edge *e)
{
    outputBinary (e -> crossTable.H64);
    outputBinary (e -> crossTable.L64);
    wprintf(L"\n");
}

bool isEpsilon(Edge *e)
{
    assert(e);
    return e -> hasEpsilon;
}


ULL128   getMatchBitVector(Edge *e)
{
    assert(e);
    return e -> crossTable;
}

void clear_status (Edge **e)
{
    assert (*e);
    (*e) -> from = (*e) -> to = NULL;
}
