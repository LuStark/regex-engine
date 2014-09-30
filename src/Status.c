#include "NFA.h"
#include "constant.h"
#include "typedef.h"
#include "Array.h"
#include "ArrayRep.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>
#include "Status.h"

#define S   Status
typedef struct S *S;


struct S
{
    Array_T     InEdges;
    Array_T     OutEdges;
    bool        FinalStatus;
    int ID;
};

Array_T allocStatusArray(int n)
{
    int i;
    Array_T statusArray;
    S   s;


    statusArray = Array_new (n, sizeof(struct S));
    for (i=0; i<Array_length(statusArray); i++)
    {
        s = Array_get(statusArray, i);
        s->ID = 0;
        s->FinalStatus = false;
        //s->InEdges = NULL;
        //s->OutEdges = NULL;
        s->InEdges = Array_new(0, sizeof(int));
        s->OutEdges= Array_new(0, sizeof(int));
    }
    return statusArray;
}

void freeEdgesInStatus (S s)
{
    if (s->InEdges)
        Array_free (&s->InEdges);

    if (s->OutEdges)
        Array_free (&s->OutEdges);

    s->InEdges = NULL;
    s->OutEdges = NULL;
}

void clearEdges(S s)
{
    s->InEdges = s->OutEdges = NULL;
}

int sizeOfStatus()
{
    return sizeof(struct S);
}


S    allocStatus ()
{
    S s;
    s = malloc (sizeof (struct S));
    assert(s);
   
    s->InEdges = Array_new (0, sizeof(int));
    s->OutEdges = Array_new (0, sizeof(int));
    return s;
}

void initInEdges(S s)
{
    assert(s);
    s->InEdges = Array_new (0, sizeof(int));
}

void initOutEdges(S s)
{
    assert(s);
    s->OutEdges = Array_new (0, sizeof(int));
}

int  getStatusID(S s)
{
    assert(s);
    return s->ID;
}

Array_T getInEdges(S s)
{
    assert(s);
    return s->InEdges;
}

Array_T getOutEdges(S s)
{
    assert(s);
    return s->OutEdges;
}

void printStatus (S s)
{

}

void setStatusID (S s, int id)
{
    assert(s);
    s->ID = id;
}

void ensureFinalStatus (S s)
{
    assert(s);
    s->FinalStatus = true;
}

void cancelFinalStatus (S s)
{
    assert(s);
    s->FinalStatus = false;
}

void appendInEdge (S s, int e)
{
    assert(s);
    Array_append (s->InEdges, &e); 
}

void appendOutEdge (S s, int e)
{
    assert(s);
    Array_append (s->OutEdges, &e);
}

bool  isFinalStatus (S s)
{
    assert(s);
    return s->FinalStatus;
}


