#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "FirstFollow.h"
#include "NFA.h"
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
    regex[i++] = '$';
    regex[i] = '\0';
}

int match (wchar_t obj)
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

int LL1_non_special_wchar_t()
{
    int p;
    wchar_t c;
    
    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        match (c);
    }
    else
        error (L"LL1_noncharacter()", c);

    return p;
}

int LL1_escapecharacter()
{
    wchar_t    c;
    int  p;

    c = regex[currentIndex];
    if (c != '\\')
        error (L"LL1_noncharacter()", c);

    match ('\\');
    c = regex[currentIndex];
    match (c);

    //p = CreateOneNFA (c);

    return p;
}

int LL1_character()
{
    wchar_t    c;
    int  p; 
   
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

int LL1_character_range()
{
    wchar_t c;
    Range r, bufferRange[280];
    int non, sizeBuffer;
    Status  *start, *end;
    Edge    *rangeE;

    start = allocStatus ();
    end   = allocStatus ();
    rangeE = allocEmptyEdge ();
    
    match ('[');
    non = LL1_choose_or_not();

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

    if (non == 1)
    {
    }
    else
    {

    }

    match (']');
}

int LL1_regex();

int LL1_re_top_level()
{
    wchar_t c;

    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        LL1_non_special_wchar_t();
    }
    else if (c == '(')
    {
        LL1_regex();
    }
    else if (c == '\\')
    {
        LL1_escapecharacter();
    }
    else if (c == '[')
    {
        LL1_character_range();
    }
    else
    {
        error (L"re_top_level()", c);
    }
}

int LL1_closure_op()
{
    wchar_t c;
    c = regex[currentIndex];

    match (c);

}

int LL1_re_closure_level()
{
    wchar_t c;
    
    LL1_re_top_level();
    c = regex[currentIndex];
    if (First_closure_op[c] == 1)
    {
        LL1_closure_op();
    }
}

int LL1_re_link_level()
{
    wchar_t c;

    LL1_re_closure_level();
    
    c = regex[currentIndex];
    while (First_re_closure_level[c] == 1)
    {
        LL1_re_closure_level();
        c = regex[currentIndex];
    }

}

int LL1_re_union_level()
{
    wchar_t c;

    LL1_re_link_level();
    c = regex[currentIndex];
    while (c == '|')
    {
        match ('|');
        LL1_re_link_level();
        c = regex[currentIndex];
    }
}

int LL1_regex()
{
    wchar_t c;

    LL1_re_union_level();

    if (!LL1_finished_symbol)
    {
        wprintf (L"未可预知的符号: ");
        while ((c=regex[currentIndex++]) != '$')
            putwchar (c);
        exit(1);
    }
}

int main ()
{
    setlocale(LC_CTYPE, "");
    init_FirstSet();
    init_FollowSet();
    getRegex();

    LL1_regex();
}
