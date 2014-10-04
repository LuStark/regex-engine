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

void
construct_table(regexNode re)
{
    re->dfa = Subset_Construct(re->nfa_buffer);
    re->T   = makeUpDFATable(re->dfa);
}

regexNode
alloc_regexNode()
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

/* 一旦成功匹配regex，立即返回下标位置，若匹配失败，则返回-1 */
int
greedy_match(regexNode re, wchar_t *str, int currPos)
{
    int i;
    int length = wcslen(str);
    wchar_t     c;
    wchar_t     *p;
    bool        match;

    p = str+currPos;
    i = 0;
    while (*p)
    {
        if (re->T[i][*p] == -1)
          return -1;
        else
          i = re->T[i][*p];
        if (isFinalStatus(Array_get(re->dfa->statusArray, i)))
        {
            match = 1;
        }
        /* 前移直到无法匹配为止 */
        if (match && (!isFinalStatus(Array_get(re->dfa->statusArray, i)) || (*(p+1))=='\0'))
            return p-str-currPos+1;
        p++;
    }
    return -1;
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

bool
re_match2(regexNode re, wchar_t *str, int *start, int *end)
{
    char    op;
    int     b1, b2;
    int     s, e;
    int     len;

    if (!re)
      return false;

    if (!re->left && !re->right)
    {
        len = greedy_match(re, str, *end);
        if (len==-1) return false;
        /* 否则就说明匹配re成功 */
        *end = *start + len;
        return true;
    }
    
    op = re->regex_op;
    if (op == '|')
    {
        b1 = re_match2(re->left, str, start, end);
        if (b1) return true;

        b2 = re_match2(re->right, str, start, end);
        if (b2) return true;
    }
    else if (op == '&')
    {
        /* 先匹配左边的正则表达式，再匹配右边的正则表达式 */
        b1 = re_match2(re->left, str, start, end);
        if (re->left->type == BOUND)
            *start = *end;
        if (!b1) return false;
        
        b2 = re_match2(re->right, str, start, end);
        if (re->right->type== BOUND)
            *end = *start;

        if (!b2)
            return false;
        else
            return true;
    }

    return false;
}

void
matchContext(wchar_t *T, regexNode re)
{
    int start, end, currPos;

    int n;

    currPos = start = end = 0;
    n = wcslen(T);

    while (currPos < n)
    {
        if (re_match2(re, T, &start, &end))
        {
            wprintf(L"匹配成功，识别边界为[%d, %d)\n", start, end);
            return;
        }
        else
        {
            currPos++;
            start = end = currPos;
        }

    }
    wprintf(L"匹配失败\n");

}
