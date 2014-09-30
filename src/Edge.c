#include "NFA.h"
#include "Edge.h"
#include "constant.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>


typedef unsigned long long int ULL64;

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


#define E   Edge 
typedef struct E *E;

struct E
{
    ULL128      crossTable;
    bool        is_match;
    Status      from;
    Status      to;

    bool        hasEpsilon;
    Array_T     content;
    bool        anymatch;
};

ULL128   getMatchBitVector(E e)
{
    assert(e);
    return e->crossTable;
}

bool isEpsilon(E e)
{
    assert(e);
    return e->hasEpsilon;
}

Array_T allocEdgeArray(int n)
{
    int i, j;
    E   e;
    Array_T edgeArray;

    edgeArray = Array_new (n, sizeof(struct E));
    
    for (i=0; i<Array_length(edgeArray); i++)
    {
        e = Array_get(edgeArray, i);
        e->from = e->to = NULL;
        e->crossTable.L64 = 0L;
        e->crossTable.H64 = 0L;
        e->hasEpsilon   = false;
        e->is_match     = true;
        e->content      = Array_new(0, sizeof(wchar_t));
    }
    return edgeArray;
}

Array_T getEdgeContent(E e)
{
    assert(e->content);
    return e->content;
}

void unreferenceStatus (E e)
{
    e->from=NULL;
    e->to = NULL;
}

void clearBits(E e)
{
    assert(e);
    e->crossTable.L64=0L;
    e->crossTable.H64=0L;
    e->is_match = true;
    e->hasEpsilon = false;
}

int  sizeOfEdge()
{
    return sizeof(struct E);
}

void copyEdge_without_Status (E to, E from)
{
    to->crossTable = from->crossTable;
    to->is_match = from->is_match;
    to->hasEpsilon = from->hasEpsilon;
}

void outputEdgeCrossTable (E e)
{
    outputBinary (e->crossTable.H64);
    outputBinary (e->crossTable.L64);
    wprintf(L"\n");
}


void addCharacter (E e, wchar_t c)
{
    if (c >= 128)
        wprintf(L"暂时不处理非ASCII码");
    
    if (c >= 64 )
        e -> crossTable.H64 |= (1ULL << (c-64));
    else
        e -> crossTable.L64 |= (1ULL << c);

    Array_append(e->content, &c);
    
}

void setFromToStatus (E e, Status from, Status to)
{
    assert(e);

    e->from = from;
    e->to = to;
}

void clearStatus (E e)
{
    assert (e);
    e->from = e->to = NULL;
}


void addRange (E e, Range r)
{
    wchar_t c1, c2, c;
    c1 = r.from; c2 = r.to;
    if (c2<c1)
        return;
    int i;

    if (c1 < 64 && c2 >= 64)
    {
        e -> crossTable.L64 = (~(1ULL<<c1)+1);
        e -> crossTable.H64 = (1ULL<<(c2-64)-1);
        e -> crossTable.H64 |= (1ULL<<(c2-64));
    }
    else
    {
        if (c1 >= 64)
        {
            c1 -= 64;
            c2 -= 64;
            e -> crossTable.H64 |= (1ULL<<c2)-(1ULL<<c1);
            e -> crossTable.H64 |= (1ULL<<c2);
        }
        else
        {
            e -> crossTable.L64 |= (1ULL<<c2)-(1ULL<<c1);
            e -> crossTable.L64 |= (1ULL<<c2);
        }
    }
    for (c=c1; c<=c2; c++)
      Array_append(e->content, &c);
}

int crossEdge (E e, wchar_t c)
{
    if (c < 64)
        return (e->crossTable.L64 & (1ULL<<c)) > 0;
    else
        return (e->crossTable.H64 & (1ULL<<c-64)) > 0;
}

E   allocEdge ()
{   
    E e = malloc (sizeof(struct E));
    assert (e);
    
    e->crossTable.L64 = 0L;
    e->crossTable.H64 = 0L;
    e -> is_match   = true;
    e -> content    = Array_new(0, sizeof(wchar_t));
    return e;
}

E   allocEpsilonEdge ()
{
    E e = allocEdge ();
    e -> hasEpsilon = true;
    e->crossTable.L64 = 0L;
    e->crossTable.H64 = 0L;
    e -> is_match   = true;
    e -> content    = Array_new(0, sizeof(wchar_t));

    return e;
}

void freeEdge(E e)
{
    Array_free(&e->content);
    e->from = e->to = NULL;    
    e->crossTable.L64=0L;
    e->crossTable.H64=0L;
    e->is_match = 1;
    e->hasEpsilon = false;
    //free(e);
}


void setEpsilon(E e)
{
    assert(e);
    e->hasEpsilon = true;
}


void setMatchRangeOrNot (E e, int c)
{
    e->is_match = c;
}

void printEdge (E e)
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

Status getfromStatus (E e)
{
    assert (e);
    return e->from;
}

Status gettoStatus (E e)
{
    assert (e);
    return e->to;
}


#undef E


