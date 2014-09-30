#include "DFA.h"
#include "NFA.h"
#include "StatusSet.h"
#include "automaton.h"
#include <stdlib.h>
#include <assert.h>


static void make_connection(DFA dfa, int *numOfEdges, int from, int to, wchar_t c)
{
    Edge    e;

    if ((e=getLink(dfa, from, to)) != NULL)
    {
        addCharacter(e, c);
        return;
    }

    e = allocEdge();
    clearBits(e);
    addCharacter(e, c);
    addEdge(dfa, e);
    e = Array_get(dfa->edgeArray, *numOfEdges);

    link_Two_Status_In_Automaton (dfa, from, to, *numOfEdges);
    (*numOfEdges)++;
}

/* 可能要考虑到from的outedges中某边覆盖不止字符c */
static bool exist_connection(DFA dfa, int from, int to, wchar_t c)
{
    int too;
    
    too = reachStatus(dfa, from, c);
    return too==to;
}

extern DFA Subset_Construct (NFA nfa)
{
    StatusSet_table T;
    StatusSet   currSet, toSet;
    Edge    e;
    DFA     dfa;

    int     reverse[200], size;
    /* 暂时不针对中文进行处理 */
    wchar_t c;
    Array_T toSets, charSet;
    int     i,j, numOfEdges;
    int Q[500], front, tail, term, k, from, to, ***linkcheck;

    numOfEdges = 0;
    front = tail = 0;
    Q[tail++] = 0;

    charSet = getCharSet (nfa);

    /*
    for (i=0; i<Array_length(charSet); i++)
        wprintf(L"%c ", *(wchar_t*)Array_get(charSet, i));
    wprintf(L"\n");
    */

    get_two_StatusSet_Table (nfa, &T, reverse);
    dfa = CreateDFA (T.size, 0);

    wprintf(L"\n");
    while (front != tail)
    {
        from = Q[front++];
        currSet = T.H[from];
        //printStatusSet(currSet);
        for (i=0; i<Array_length(charSet); i++)
        {
            c = *(wchar_t*)Array_get(charSet, i);
            toSets = reach_Status (nfa, currSet, reverse, c);
            for (j=0; j<Array_length(toSets); j++)
            {
                to = *(int*)Array_get(toSets, j);
                
                if (exist_connection(dfa, from, to, c))
                  continue;

                toSet = T.H[to];
                //wprintf(L"%d -> %d, by = %c\n", from, to, c);
                
                /* numOfEdges是当前添加的边的数目 */
                make_connection(dfa, &numOfEdges, from, to, c);

                if (currSet.hasFinalStatus)
                    ensureFinalStatus(Array_get(dfa->statusArray, from));

                if (toSet.hasFinalStatus)
                    ensureFinalStatus(Array_get(dfa->statusArray, to));

                Q[tail++] = to;
            }
        }
    }
    wprintf(L"\n");
    return dfa;
}

extern DFA  CreateDFA(int n, int e)
{
    int i;
    Status  s;

    assert(n>0);
    assert(e>=0);

    DFA p;
    p = malloc(sizeof(struct Automaton));
    assert(p);

    if (e>0)
        p->edgeArray = allocEdgeArray(e);
    else
        p->edgeArray = Array_new(0,sizeOfEdge());

    p->statusArray = allocStatusArray(n);
    if (n==1)
        p->start = p->end = Array_get(p->statusArray, 0);
    else
    {
        p->start = Array_get(p->statusArray, 0);
        p->end  = Array_get(p->statusArray, n-1);
    }
    for (i=0; i<Array_length(p->statusArray); i++)
    {
        s = Array_get(p->statusArray, i);
    }
    

    adjustStatusID(p);
    return p;
}

void 
addEdge(DFA dfa, Edge e)
{
    assert(e);
    assert(dfa->edgeArray);
    Array_append(dfa->edgeArray, e);
}
