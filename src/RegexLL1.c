#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "FirstFollow.h"
#include "NFA.h"
#include "Edge.h"
#include "Status.h"
#include <locale.h>


#define MAX 150

wchar_t regex[MAX];
int  currentIndex= 0;
int  LL1_finished_symbol = 0;

void getRegex ()
{
    int i;
    wchar_t c;

    i = 0;
    while ((c=getwchar()) != EOF)
        regex[i++] = c;
    if (i>0 && regex[i-1]=='\n')
        i--;
    regex[i++] = '$';
    regex[i] = '\0';
}

void match (wchar_t obj)
{
    if (LL1_finished_symbol==1)
        return;
    if (regex[currentIndex]==obj)
    {
        wprintf (L"匹配 %lc\n", obj);
        currentIndex++;
        if (regex[currentIndex]=='$')
        {
            wprintf (L"匹配完成.\n");
            LL1_finished_symbol = 1;
        }
    }
    else
    {
        wprintf (L"%lc Don't match %lc\n", regex[currentIndex], obj);
        exit(1);
    }
}

int error (wchar_t funcName[], wchar_t c)
{
    wprintf (L"%ls中出现无法匹配的字符%c\n", funcName, c);
    exit (1);
}

NFA  LL1_non_special_wchar_t()
{
    wchar_t c;
    NFA  p;
    
    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        match (c);
        p = CreateSingleNFA (c);
    }
    else
        error (L"LL1_noncharacter()", c);

    return p;
}

NFA  LL1_escapecharacter()
{
    wchar_t    c;
    NFA  p;

    c = regex[currentIndex];
    if (c != '\\')
        error (L"LL1_noncharacter()", c);

    match ('\\');
    c = regex[currentIndex];
    p = CreateSingleNFA (c);
    match (c);

    return p;
}

NFA  LL1_character()
{
    wchar_t    c;
    NFA  p; 
   
    c = regex[currentIndex];
    if (First_character[c]==1)
    {
        if (c=='\\')
            p = LL1_escapecharacter();
        else
            p = LL1_non_special_wchar_t();
    }
    else
        error (L"LL1_character()", c);

    return p;
}

int LL1_choose_or_not()
{
    wchar_t c;
    c = regex[currentIndex];
    
    if (Follow_choose_or_not[c] == 1)
        return 0;

    if (c == '^')
        match ('^');
    else
        error (L"LL1_character()", c);
}

Range LL1_range()
{
    wchar_t c1, c2;
    Range   r;
    
    c1 = regex[currentIndex];
    if (First_range[c1] == 1)
    {
        match (c1);
        if (regex[currentIndex] == '-')
        {
            match ('-');
            c2 = regex[currentIndex];
            if (c2 != ']')
            {
                match (c2);
                r.from = c1;
                r.to = c2;
            }
            else
            {
                wprintf(L"范围不应该包含']'\n");
                exit(1);
            }
        }
        else
        {
            /* 如果只有单个字符，范围上下界取该字符 */
            r.from = c1;
            r.to = c1;
        }
    }
    else
        error (L"LL1_range()", c1);

    return r;
}

NFA LL1_character_range()
{
    wchar_t c;
    Range r, bufferRange[280];
    int sizeBuffer;
    Status  start, end;
    Edge   e;
    NFA  p;
    int     i;

    // 创建两个顶点一条边的NFA
    p = CreateNFA (2,1);
    start = getStartStatus (p);
    end   = getEndStatus (p);
    e     = getEdge (p,0);
    
    match ('[');

    setMatchRangeOrNot (e, LL1_choose_or_not());

    c = regex[currentIndex];
    sizeBuffer = 0;
    while (First_range[c] == 1)
    {
        r = LL1_range();
        bufferRange[sizeBuffer++] =  r;
        
        c = regex[currentIndex];
        if (c==']')
            break;
    }

    for (i=0; i<sizeBuffer; i++)
        addRange (e, bufferRange[i]);

    match (']');

    linkTwoStatusInNFA (p, 0, 1, 0);

    return p;
}

//declaration
NFA LL1_regex();

NFA LL1_re_top_level()
{
    NFA p;
    wchar_t c;

    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        p = LL1_non_special_wchar_t();
    }
    else if (c == '(')
    {
        match ('(');
        p = LL1_regex();
        match (')');
    }
    else if (c == '\\')
    {
        p = LL1_escapecharacter();
    }
    else if (c == '[')
    {
        p = LL1_character_range();
    }
    else
    {
        error (L"re_top_level()", c);
    }

    return p;
}

wchar_t LL1_closure_op()
{
    wchar_t c;
    c = regex[currentIndex];

    match (c);
    return c;
}

NFA LL1_re_closure_level()
{
    wchar_t c, op;
    NFA nfa, nfa2;
    
    nfa = LL1_re_top_level();
    c = regex[currentIndex];
    if (First_closure_op[c] == 1)
    {
        op = LL1_closure_op();
        if (op=='*')
            nfa2 = Closure (nfa);
        else if (op=='+')
            nfa2 = Repeat_atleast_one(nfa);
        else if (op=='?')
            nfa2 = Option (nfa);
        else
            wprintf(L"无法识别的闭包运算符\n");
        nfa = nfa2;
    }

    return nfa;
}

NFA LL1_re_link_level()
{
    wchar_t c;
    NFA nfa1, nfa2, nfa;

    nfa1 = LL1_re_closure_level();
    
    c = regex[currentIndex];
    while (First_re_closure_level[c] == 1)
    {
        nfa2 = LL1_re_closure_level();
        //printNFA(nfa2);
        c = regex[currentIndex];
        nfa = Link(nfa1, nfa2);
        nfa1 = nfa;
    }

    return nfa1;
}

NFA LL1_re_union_level()
{
    wchar_t c;
    NFA nfa1, nfa2, nfa;

    nfa1 = LL1_re_link_level();
    c = regex[currentIndex];
    while (c == '|')
    {
        match ('|');
        nfa2 = LL1_re_link_level();
        nfa = Union(nfa1, nfa2);
        nfa1 = nfa;
        c = regex[currentIndex];
    }
    return nfa1;
}

NFA LL1_regex()
{
    wchar_t c;
    NFA nfa;

    nfa = LL1_re_union_level();

    if (!LL1_finished_symbol)
    {
        wprintf (L"未可预知的符号: ");
        while ((c=regex[currentIndex++]) != '$')
            putwchar (c);
        exit(1);
    }

    return nfa;
}

int main ()
{
    int i;
    NFA nfa;
    setlocale(LC_CTYPE, "");
    
    init_FirstSet();
    init_FollowSet();
    getRegex();

    nfa = LL1_regex();

    printNFA (nfa);

    return 0;
}
