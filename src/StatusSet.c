#include "StatusSet.h"
#include "automaton.h"


static void DFS(Status, NFA nfa, Array_T, int[], int[], int);

static void getEpsilonClosure(Status s, NFA nfa, Array_T closure, int inverse[], int set)
{
    int i, color[300];
    for (i=0; i<300; i++)   color[i] = 0;

    DFS(s, nfa, closure, inverse, color, set);
}

static void DFS(Status fromS, NFA nfa, Array_T closure, int inverse[], int color[], int set)
{
    int     i, index, from, to;
    Edge    e;
    Status  toS;

    from = getStatusID(fromS);
    color[from] = 1;
    inverse[from] = set;
    Array_append(closure, &from);
    
    Array_T edgeArray   =   getEdgeArray(nfa);
    Array_T outEdges_indice = getOutEdges(fromS);
    if (!outEdges_indice)
        return;

    for (i=0; i<Array_length(outEdges_indice); i++)
    {
        index   =   *(int*)Array_get(outEdges_indice, i);  
        e       =   (Edge)Array_get(edgeArray, index);
        if (isEpsilon(e))
        {
            toS     =   gettoStatus(e);
            to      =   getStatusID(toS);
            if (color[to] == 0)
                DFS(toS, nfa, closure, inverse, color, set);
        }
    }
}


void get_two_StatusSet_Table (NFA nfa, StatusSet_table *T, int inverse[])
{
    int     i, j, id, n, curr_set_id, e_index;
    Edge    e;
    Status  s;
    Array_T StatusArray, edgeArray, OutEdges, closure;
 

    T->size = 0;
    StatusArray = getStatusArray (nfa);
    edgeArray   = getEdgeArray (nfa);
    n = Array_length (StatusArray);
 
    /* 一开始所有状态不属于任一集合 */
    for (i=0; i<n; i++) inverse[i] = -1;

    for (i=0; i<n; i++)
    {
        if (inverse[i] != -1)
            continue;
        /* 闭包缓存 */
        curr_set_id = T->size;

        inverse[i] = curr_set_id;

        closure = Array_new (0, sizeof(int));
        s = Array_get(StatusArray, i);

        getEpsilonClosure(s, nfa, closure, inverse, curr_set_id);
        for (j=0; j<Array_length(closure); j++)
        {
            id = *(int*)Array_get(closure, j);
            s = Array_get(nfa->statusArray, id);
            if (isFinalStatus(s))
              T->H[T->size].hasFinalStatus = true;
        }

        /* 以s为原点进行DFS，找出所有epsilon闭包 
         * getEpsilonClosure(s, closure, inverse)
         */
        T->H[T->size].epsClosure = closure;
        //printStatusSet(T->H[T->size]);
        //wprintf(L"-------------------------------\n");

        T->size++;
    }

}

Array_T reach_Status (NFA nfa, StatusSet currSet, int inverse[], wchar_t c)
{
    Array_T toSets;
    int  from, to;

    toSets = Array_new(0,sizeof(int));

    int i;

    for (i=0; i<Array_length(currSet.epsClosure); i++)
    {
        from = *(int*)Array_get(currSet.epsClosure, i);
        to = reachStatus(nfa, from, c);
        if (to!=-1)
        {
            Array_append(toSets, &inverse[to]);
        }
    }

    return toSets;
}


void printStatusSet(StatusSet S)
{
    int i;
    wprintf(L"eps_Closure: ");
    for (i=0; i<Array_length(S.epsClosure); i++)
        wprintf(L"%d ", *(int*)Array_get(S.epsClosure, i));
    wprintf(L"\n");
}
