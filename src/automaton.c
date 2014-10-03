#include "constant.h"
#include "typedef.h"
#include "automaton.h"
#include "Edge.h"

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>

void     adjustStatusID  (Automaton *AM)
{
    int i,n;
    Status s;

    n = Array_length(AM->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get(AM->statusArray, i);
        setStatusID(s, i);
    }
    AM->start = Array_get(AM->statusArray, 0);
    AM->end = Array_get(AM->statusArray, n-1);

    ensureFinalStatus (AM->end);
}

void     link_Two_Status_In_Automaton (Automaton *AM, int from, int to, int e)
{
    assert(AM);
    
    Status      fromS, toS;
    Edge        bridge;
    Array_T     Edges;

    fromS   = Array_get (AM->statusArray, from);
    toS     = Array_get (AM->statusArray, to);
    bridge  = Array_get (AM->edgeArray, e);
    
    assert(fromS);
    assert(toS);
    assert(bridge);

    setFromToStatus (bridge, fromS, toS);
    
//    if (!getOutEdges(fromS))
//        initOutEdges(fromS);
    appendOutEdge(fromS, e);

//    if (!getInEdges(toS))
//        initInEdges(toS);
    appendInEdge(toS, e);

}

void     print_Automaton (Automaton *AM)
{
    int i;
    Edge e;
    Status s1, s2;

    s1 = AM->start;
    if (isFinalStatus(s1))
    {
        wprintf(L"(0)         (F)\n");
    }

    for (i=0; i<Array_length(AM->edgeArray); i++)
    {
        e = Array_get (AM->edgeArray, i);
        s1 = getfromStatus (e);
        s2 = gettoStatus (e);
        wprintf(L"(%d, %d, ", getStatusID(s1), getStatusID(s2));
        printEdge(e);
        wprintf(L")");
        if (isFinalStatus(s2))
        {
            wprintf(L"   (F)");
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
}

void     free_Automaton  (Automaton *AM)
{
    Edge    e;
    int i;
    assert(AM);
    
    for (i=0; i < Array_length(AM->edgeArray); i++)
    {
        e = Array_get(AM->edgeArray, i);
        freeEdge(e);
        e = NULL;
    }

    for (i=0; i<Array_length(AM->statusArray); i++)
        freeEdgesInStatus (Array_get(AM->statusArray, i));

    if (AM -> statusArray)
        Array_free (&AM->statusArray);

    if (AM->edgeArray)
        Array_free (&AM->edgeArray);
    free (AM);

}

Array_T  getStatusArray  (Automaton *AM)
{
    assert(AM);
    return AM->statusArray;
}

Array_T  getEdgeArray    (Automaton *AM)
{
    assert(AM);
    return AM->edgeArray;
}

Status   getStartStatus (Automaton *AM)
{
    assert(AM);
    assert(AM->start);
    return AM->start;
}

Status   getEndStatus (Automaton *AM)
{
    assert(AM);
    assert(AM->end);
    return AM->end;
}
Edge     getEdge (Automaton *AM, int i)
{
    assert(AM);
    assert(AM->edgeArray);
    assert(i<Array_length(AM->edgeArray) && i>=0);

    return Array_get (AM->edgeArray, i);
    
}
Status   getStatus (Automaton *AM, int i)
{
    assert(AM);
    assert(AM->statusArray);
    assert(i<Array_length(AM->statusArray) && i>=0);

    return Array_get (AM->statusArray, i);
}

Edge     getLink (Automaton *AM, int from, int to)
{
    int     i, e_index;
    Status  s;
    Edge    e;
    s = Array_get(AM->statusArray, from);
        
    Array_T outs = getOutEdges(s);
    for (i=0; i<Array_length(outs); i++)
    {
        e_index =   *(int*)Array_get(outs, i);
        e   =   Array_get(AM->edgeArray, e_index);
        if (getStatusID(gettoStatus(e))==to)
            return e;
    }
    return NULL;
}

bool     isYuCha (Automaton *AM)
{
    return AM->yucha;
}
