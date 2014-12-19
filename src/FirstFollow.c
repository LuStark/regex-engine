#include "FirstFollow.h"
#include <string.h>


int First_Regex[128];
int First_Regex_union[128];
int First_Regex_link[128];
int First_Regex_closure[128];
int First_Regex_top[128];
int First_normal_char[128];
int First_escape_char[128];
int First_char_range[128];
int First_choose_or_not[128];
int First_range[128];
int First_extend_regex[128];
int First_string[128];

void initFollowSet();

int Follow_Regex[128] = {0};
int Follow_Regex_union[128] = {0};
int Follow_Regex_link[128] = {0};
int Follow_Regex_closure[128] = {0};
int Follow_Regex_top[128] = {0};
int Follow_normal_char[128] = {0};
int Follow_escape_char[128] = {0};
int Follow_char_range[128] = {0};
int Follow_choose_or_not[128] = {0};
int Follow_range[128] = {0};
int Follow_extend_regex[128] = {0};
int Follow_string[128] = {0};

void initFirstSet()
{
    int i;
    for (i=0; i<128; i++)
    {
        if (i == '$')
            continue;
        First_range[i] = 1;
        First_Regex_top[i] = 1;
        First_normal_char[i] = 1;
        First_escape_char[i] = 0;
        First_choose_or_not[i]=0;
        First_char_range[i] = 0;
        First_string[i] = 1;
    }
    
    First_Regex_top['$'] = First_Regex_top['+'] = 
            First_Regex_top['*'] = First_Regex_top['?'] = 
                First_Regex_top['|'] = First_Regex_top[')'] = 0;
    for (i=0; i<128; i++)
    {
        First_Regex[i] = First_extend_regex[i] = First_Regex_union[i] = 
            First_Regex_link[i] = First_Regex_closure[i] = First_Regex_top[i];
    }


    First_normal_char['\\'] = 0;
    First_normal_char['+'] = 0;
    First_normal_char['*'] = 0;
    First_normal_char['?'] = 0;
    First_normal_char['|'] = 0;
    First_normal_char['['] = 0;
    First_normal_char['('] = 0;
    
    First_choose_or_not['^'] = 1;
    First_char_range['['] = 1;
    First_range[']'] = 0;
    First_escape_char['\\']= 1;
    
    First_string['>'] = 0;
}

void initFollowSet()
{
    int i;

    Follow_Regex['$'] = 1;
    Follow_Regex[')'] = 1;

    Follow_Regex_union['$'] = 1;
    Follow_Regex_union[')'] = 1;

    Follow_Regex_link['$'] = 1;
    Follow_Regex_link['|'] = 1;
    Follow_Regex_link[')'] = 1;
    
    Follow_Regex_closure['$'] = 1;
    Follow_Regex_closure['|'] = 1;
    Follow_Regex_closure[')'] = 1;

    Follow_Regex_top['$'] = 1;
    Follow_Regex_top['|'] = 1;
    Follow_Regex_top['*'] = 1;
    Follow_Regex_top['+'] = 1;
    Follow_Regex_top['?'] = 1;
    Follow_Regex_top['{'] = 1;
    Follow_Regex_top[')'] = 1;


    /* 非特殊字符的下一个符号可能是:  $, |, *, +, ?, ) */
    Follow_normal_char['$'] = 1;
    Follow_normal_char['|'] = 1;
    Follow_normal_char['*'] = 1;
    Follow_normal_char['+'] = 1;
    Follow_normal_char['?'] = 1;
    Follow_normal_char[')'] = 1;
    Follow_normal_char['{'] = 1;

    Follow_escape_char['$'] = 1;
    Follow_escape_char['|'] = 1;
    Follow_escape_char['*'] = 1;
    Follow_escape_char['+'] = 1;
    Follow_escape_char['?'] = 1;
    Follow_escape_char[')'] = 1;
    Follow_escape_char['{'] = 1;

    Follow_char_range['$'] = 1;
    Follow_char_range['|'] = 1;
    Follow_char_range['*'] = 1;
    Follow_char_range['+'] = 1;
    Follow_char_range['?'] = 1;
    Follow_char_range[')'] = 1;
    Follow_char_range['{'] = 1;

    Follow_extend_regex[')'] = 1;
    Follow_string['>'] = 1;

    for (i=0; i<128; i++)
    {
        Follow_choose_or_not[i] = 1;
        Follow_range[i] = 1;
    }
    Follow_choose_or_not['$'] = 0;

}
