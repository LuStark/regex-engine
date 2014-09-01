#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FirstFollow.h"

#define MAX 150

char regex[MAX];
int  currentIndex= 0;
int  LL1_finished_symbol = 0;

void getRegex ()
{
    int i;
    char c;

    i = 0;
    while ((c=getchar())!=EOF)
        regex[i++] = c;
    regex[i++] = '$';
    regex[i] = '\0';
}

int match (char obj)
{
    if (LL1_finished_symbol==1)
        return;
    if (regex[currentIndex]==obj)
    {
        printf ("匹配 %c\n", obj);
        currentIndex++;
        if (regex[currentIndex]=='$')
        {
            printf("匹配完成.\n");
            LL1_finished_symbol = 1;
        }
    }
    else
    {
        printf ("%c Don't match %c\n", regex[currentIndex], obj);
        exit(1);
    }
}

int error (char funcName[], char c)
{
    printf ("%s中出现无法匹配的字符%c\n", funcName, c);
    exit (1);
}

int LL1_non_special_char()
{
    char c;
    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
        match (c);
    else
        error ("LL1_non_character()", c);

}

int LL1_escape_character()
{
    char c;
    c = regex[currentIndex];
    
    if (c != '\\')
        error ("LL1_non_character()", c);
    
    match('\\');
    match (regex[currentIndex]);
}

int LL1_character()
{
    char c;
    c = regex[currentIndex];
   
    if (First_character[c]==1)
    {
        if (c=='\\')
            LL1_escape_character();
        else
            LL1_non_special_char();
    }
    else
        error ("LL1_character()", c);
}

int LL1_choose_or_not()
{
    char c;
    c = regex[currentIndex];
    
    if (Follow_choose_or_not[c] == 1)
        return 0;

    if (c == '^')
        match ('^');
    else
        error ("LL1_character()", c);
}

int LL1_range()
{
    char c1, c2;
    
    c1 = regex[currentIndex];
    if (First_range[c1] == 1)
    {
        LL1_character();
        if (regex[currentIndex] == '-')
        {
            match ('-');
            c2 = regex[currentIndex];
            if (c2 != ']')
            {
                LL1_character();
            }
            else
            {
                printf("范围不应该包含']'\n");
            }
        }
        else
        {

        }
    }
    else
        error ("LL1_range()", c1);
}

int LL1_character_range()
{
    char c;
    
    match ('[');
    LL1_choose_or_not();

    c = regex[currentIndex];
    while (First_range[c] == 1)
    {
        LL1_range();
        c = regex[currentIndex];
        if (c==']')
            break;
    }
    match (']');
}

int LL1_regex();

int LL1_re_top_level()
{
    char c;

    c = regex[currentIndex];
    
    if (First_non_special[c] == 1)
    {
        LL1_non_special_char();
    }
    else if (c == '(')
    {
        LL1_regex();
    }
    else if (c == '\\')
    {
        LL1_escape_character();
    }
    else if (c == '[')
    {
        LL1_character_range();
    }
    else
    {
        error ("re_top_level()", c);
    }
}

int LL1_closure_op()
{
    char c;
    c = regex[currentIndex];

    match (c);

}

int LL1_re_closure_level()
{
    char c;
    
    LL1_re_top_level();
    c = regex[currentIndex];
    if (First_closure_op[c] == 1)
    {
        LL1_closure_op();
    }
}

int LL1_re_link_level()
{
    char c;

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
    char c;

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
    char c;

    LL1_re_union_level();

    if (!LL1_finished_symbol)
    {
        printf ("未可预知的符号: ");
        while ((c=regex[currentIndex++]) != '$')
            putchar (c);
        exit(1);
    }
}

int main ()
{
    init_FirstSet();
    init_FollowSet();
    getRegex();

    LL1_regex();
}
