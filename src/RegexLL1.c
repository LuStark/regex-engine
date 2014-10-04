#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include "FirstFollow.h"
#include "NFA.h"
#include "Edge.h"
#include "Status.h"
#include <locale.h>
#include "StatusSet.h"
#include "DFA.h"
#include "automaton.h"
#include "Regex.h"
#include "RegexLL1.h"

#define MAX 150

wchar_t regex[MAX];
int  currentIndex= 0;
int  LL1_finished_symbol = 0;

void getRegex ()
{
    int i;
    wchar_t c;

    i = 0;
    while ((c=getwchar()) != '\n')
        regex[i++] = c;
    //if (i>0 && regex[i-1]=='\n') i--;
    regex[i++] = '$';
    regex[i] = '\0';
}

wchar_t
*getT()
{
    int     i;
    wchar_t c, *T, buffer[500];

    i = 0;
    while ((c=getwchar()) != EOF)
        buffer[i++] = c;
    buffer[i] = '\0';
    T = malloc(sizeof(wchar_t)*i);
    assert(T);
    wcscpy(T, buffer);

    return T;

}


static void match (wchar_t obj)
{
    if (LL1_finished_symbol==1)
        return;
    if (regex[currentIndex]==obj)
    {
        //wprintf (L"匹配 %lc\n", obj);
        currentIndex++;
        if (regex[currentIndex]=='$')
        {
            //wprintf (L"匹配完成.\n");
            LL1_finished_symbol = 1;
        }
    }
    else
    {
        wprintf (L"%lc Don't match %lc\n", regex[currentIndex], obj);
        exit(1);
    }
}

static int error (wchar_t funcName[], wchar_t c)
{
    wprintf (L"%ls中出现无法匹配的字符%c\n", funcName, c);
    exit (1);
}

regexNode   LL1_non_special_wchar_t()
{
    wchar_t c;
    regexNode   r;
    
    r = alloc_regexNode();

    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        match (c);
        r->nfa_buffer = CreateSingleNFA (c);
    }
    else
        error (L"LL1_noncharacter()", c);

    return r;
}

regexNode   LL1_escapecharacter()
{
    wchar_t     c;
    regexNode   r;

    r = alloc_regexNode();

    c = regex[currentIndex];
    if (c != '\\')
        error (L"LL1_noncharacter()", c);

    match ('\\');
    c = regex[currentIndex];
    r->nfa_buffer = CreateSingleNFA (c);
    match (c);

    return r;
}

regexNode   LL1_character()
{
    wchar_t     c;
    regexNode   r;
   
    c = regex[currentIndex];
    if (First_character[c]==1)
    {
        if (c=='\\')
            r = LL1_escapecharacter();
        else
            r = LL1_non_special_wchar_t();
    }
    else
        error (L"LL1_character()", c);

    return r;
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

regexNode   LL1_character_range()
{
    wchar_t c;
    Range r, bufferRange[280];
    int sizeBuffer;
    Status  start, end;
    Edge   e;
    NFA  p;
    int     i;
    regexNode   re;

    re = alloc_regexNode();

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

    link_Two_Status_In_Automaton (p, 0, 1, 0);

    re->nfa_buffer = p; 

    return re;
}

regexNode   LL1_re_boundary()
{
    wchar_t     c;
    regexNode   re;

    match('?');
    c = regex[currentIndex];
    if (c=='=')
    {
        match('=');
        re = LL1_regex();
    }
    else if (c=='<')
    {
        match('<');
        match('=');
        re = LL1_regex();
    }
    else if (c=='!')
    {
        match('!');
        match('=');
        re = LL1_regex();
    }
    construct_table(re);    

    re->type = BOUND;
    re->yucha = true;


    
    return re;
}

regexNode   LL1_re_top_level()
{
    regexNode r;
    wchar_t c;

    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        r = LL1_non_special_wchar_t();
    }
    else if (c == '(')
    {
        match ('(');
        c = regex[currentIndex];
        if (c=='?')
            r = LL1_re_boundary();
        else
            r = LL1_regex();
        match (')');
    }
    else if (c == '\\')
    {
        r = LL1_escapecharacter();
    }
    else if (c == '[')
    {
        r = LL1_character_range();
    }
    else
    {
        error (L"re_top_level()", c);
    }

    return r;
}

wchar_t     LL1_closure_op()
{
    wchar_t c;
    c = regex[currentIndex];

    match (c);
    return c;
}

regexNode   LL1_re_closure_level()
{
    wchar_t     c, op;
    regexNode   r;
    NFA         nfa, nfa2;
    
    r = LL1_re_top_level();
    c   = regex[currentIndex];
    if (First_closure_op[c] == 1)
    {
        op = LL1_closure_op();
        if (op=='*')
            nfa2 = Closure (r->nfa_buffer);
        else if (op=='+')
            nfa2 = Repeat_atleast_one(r->nfa_buffer);
        else if (op=='?')
            nfa2 = Option (r->nfa_buffer);
        else
            wprintf(L"无法识别的闭包运算符\n");
        r->nfa_buffer = nfa2;
    }

    return r;
}

regexNode   LL1_re_link_level()
{
    wchar_t     c;
    NFA         nfa1, nfa2, nfa;
    regexNode   re, re2, pre_re, post_re, op_re;
    bool        left_bound, right_bound;

    left_bound = right_bound = false;
    re = LL1_re_closure_level();
 
    c = regex[currentIndex];
    while (First_re_closure_level[c] == 1)
    {
        re2 = LL1_re_closure_level();

        if (re->type == BOUND)
        {
            pre_re = re;
            re = re2;
            left_bound = true;
            c = regex[currentIndex];
            continue;
        }
        if (re2->type == BOUND)
        {
            post_re = re2;
            right_bound = true;
            break;
        }

        nfa = Link(re->nfa_buffer, re2->nfa_buffer);
        re -> nfa_buffer = nfa;

        c = regex[currentIndex];
    }

    if (right_bound)
    {
        construct_table(re);
        construct_table(post_re);
        op_re = alloc_regexNode();
        op_re->regex_op = '&';
        op_re->left     = re;
        op_re->right    = post_re;
        re = op_re;
    }

    if (left_bound)
    {
        construct_table(re);
        construct_table(pre_re);

        op_re = alloc_regexNode();
        op_re->regex_op = '&';
        op_re->left     = pre_re;
        op_re->right    = re;
        re = op_re;
    }

    return re;
}

regexNode   LL1_re_union_level()
{
    wchar_t     c;
    regexNode   re, re2, op;

    re = LL1_re_link_level();
    c = regex[currentIndex];
    while (c == '|')
    {
        match ('|');
        re2 = LL1_re_link_level();

        if (re->type==OP || re2->type==OP)
        {

            op = alloc_regexNode();
            op->regex_op = '|';
            op->left    = re;
            op->right   = re2;
            re = op;
        }
        else
        {
            re->nfa_buffer = Union(re->nfa_buffer, re2->nfa_buffer);
        }
        c = regex[currentIndex];
    }
 
    return re;
}

regexNode   LL1_regex()
{
    wchar_t     c;
    regexNode   r;

    r = LL1_re_union_level();

    return r;
}

void cpy(char *pattern)
{
    printf("%s\n", pattern);
    exit(0);
}

void testRegex(char *testfile)
{
    wchar_t regex[150];

    FILE    *f = fopen(testfile, "r");

    /* 目前不支持空格,换行 */
    while ((fwscanf(f, L"%ls", regex)!=EOF))
    {
        regex[wcslen(regex)] = '$';
        regex[wcslen(regex)+1] = '\0';
        
        wprintf(L"%ls\n", regex);
    }
    fclose(f);

}

int main ()
{
    int         i;
    NFA         nfa;
    DFA         dfa;
    regexNode   re;
    wchar_t     str[100], pattern[100];
    wchar_t     *T;

    setlocale(LC_CTYPE, "");

    //testRegex("testCase");
 
    init_FirstSet();
    init_FollowSet();
    wprintf(L"输入正则表达式: ");
    wprintf(L"\n");
    getRegex();

    re = LL1_regex();

    wprintf(L"输入正文:");
    T = getT();
    wprintf(L"\n");
    //wchar_t *T = L"abcd";

    int start, end;
    start = 0;

    matchContext(T, re);
    /*
    if (re_match2(re, T, &start, &end))
    {
        wprintf(L"识别成功!\n");
    }
    dfa = Subset_Construct(re->nfa_buffer);
    print_Automaton(dfa);

    init_Regex(re, dfa);

    wprintf(L"输入待识别串: ");
    while (wscanf(L"%ls", str) != EOF)
        if (Recognition(*re, str))
            wprintf(L"识别成功!\n");
    */
    return 0;
}



