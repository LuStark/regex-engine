#include "Regex.h"
#include "NFA.h"
#include "typedef.h"
#include "automaton.h"
#include "RegexLL1.h"
#include <assert.h>
#include <stdlib.h>
#include "FirstFollow.h"

/* 为DFA创建一个状态转换表 T[i][j], 状态值i在字符j下转换到的状态 */

void
init_Regex(regexNode re, DFA  dfa)
{
    re->dfa  = dfa;
    re->T    = makeUpDFATable(dfa);
}

regexNode   alloc_regexNode()
{
    regexNode   re;
    re = malloc(sizeof(*re));
    assert(re);
    
    return re;
}

DFATable
makeUpDFATable (DFA dfa)
{
    int i, j, n;
    Edge    e;
    /* 针对DFA的每条边e, 其上的非匹配字符 记录在此数组中 */
    bool    isInMatchChar[128];
    wchar_t c;
    Range   range;
    int     fromID, toID;

    DFATable    T;
    n   =   Array_length(dfa->statusArray);
    T   =  malloc (sizeof(int*) * n);
    assert(T);

    for( i=0; i < n; i++ )
    {
        T[i] = malloc (sizeof(int) * 128);
        assert(T[i]);
        for (j=0; j<128; j++)  T[i][j] = -1;
    }

    for (i=0; i < Array_length(dfa->edgeArray); i++)
    {
        e = Array_get(dfa->edgeArray, i);
        fromID  =   getStatusID(getfromStatus(e));
        toID    =   getStatusID(gettoStatus(e));

        Array_T content = getEdgeContent(e);
        for (j=0; j<Array_length(content); j++)
        {
            c = *(wchar_t*)Array_get(content, j);
            T[fromID][c] = toID;
        }
    }

    return T; 
}

bool
Recognition (Regex regex, wchar_t *str)
{
    int i;
    int length = wcslen(str);
    wchar_t     c;

    i = 0;
    while ((c=(*str++)) != '\0')
    {
        if (regex.T[i][c] == -1)
          return false;
        else
          i = regex.T[i][c];
    }
    if (isFinalStatus(Array_get(regex.dfa->statusArray, i)))
        return true;

    return false;
}

regexNode
re_compile(wchar_t *pattern)
{
    regexNode   re;
 
    NFA         nfa;
    DFA         dfa;

    wchar_t _pattern[100];

    wcscpy(_pattern, pattern);
    _pattern[wcslen(_pattern)] = '$';
    _pattern[wcslen(_pattern)+1] = '\0';

    init_FirstSet();
    init_FollowSet();

    wcscpy(regex, _pattern);
    currentIndex = 0;

    re  = LL1_regex();
    re->dfa = Subset_Construct(re->nfa_buffer);
    free_Automaton(re->nfa_buffer);
    re->nfa_buffer = NULL;

    init_Regex(re, re->dfa);

    return re;
}

bool
re_match(Regex re, wchar_t *str)
{
    if (Recognition(re, str))
        return true;
    else
        return false;
}


