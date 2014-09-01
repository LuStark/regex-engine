#include "FirstFollow.h"
#include <string.h>


int First_regex[128];
int First_re_union_level[128];
int First_re_link_level[128];
int First_re_closure_level[128];
int First_re_top_level[128];
int First_non_special[128];
int First_escape_character[128];
int First_character[128];
int First_character_range[128];
int First_choose_or_not[128];
int First_range[128];
int First_closure_op[128];

void init_FollowSet();

int Follow_regex[128] = {0};
int Follow_re_union_level[128] = {0};
int Follow_re_link_level[128] = {0};
int Follow_re_closure_level[128] = {0};
int Follow_re_top_level[128] = {0};
int Follow_non_special[128] = {0};
int Follow_escape_character[128] = {0};
int Follow_character[128] = {0};
int Follow_character_range[128] = {0};
int Follow_choose_or_not[128] = {0};
int Follow_range[128] = {0};
int Follow_closure_op[128] = {0};

void init_FirstSet()
{
    int i;
    for (i=0; i<128; i++)
    {
        if (i == '$')
            continue;
        First_character[i] = 1;
        First_range[i] = 1;
        First_re_top_level[i] = 1;
        First_non_special[i] = 1;
        First_escape_character[i] = 0;
        First_choose_or_not[i]=0;
        First_character_range[i] = 0;
        First_closure_op[i] = 0;
    }
    First_re_top_level['$'] = First_re_top_level['+'] = 
            First_re_top_level['*'] = First_re_top_level['?'] = 
                First_re_top_level['|'] = 0;
    for (i=0; i<128; i++)
    {
        First_regex[i] = First_re_union_level[i] = First_re_link_level[i]
            = First_re_closure_level[i] = First_re_top_level[i];
    }


    First_non_special['\\'] = 0;
    First_character['+'] = First_non_special['+'] = 0;
    First_character['*'] = First_non_special['*'] = 0;
    First_character['/'] = First_non_special['?'] = 0;
    First_character['|'] = First_non_special['|'] = 0;
    First_character['['] = First_non_special['['] = 0;
    First_character['('] = First_non_special['('] = 0;
    
    First_choose_or_not['^'] = 1;
    First_character_range['['] = 1;
    First_range[']'] = 0;
    First_escape_character['\\']= 1;
    
    First_closure_op['*'] = 1;
    First_closure_op['+'] = 1;
    First_closure_op['?'] = 1;


}

void init_FollowSet()
{
    int i;
    for (i=0; i<128; i++)
    {
        Follow_character[i] = 1;
        Follow_choose_or_not[i] = 1;
    }
    Follow_character['$'] = 0;
    Follow_choose_or_not['$'] = 0;
    
    Follow_regex['$'] = Follow_regex[')'] = 1;
    Follow_re_union_level['$'] = Follow_re_union_level[')'] = 1;
    Follow_re_link_level['$'] = Follow_re_link_level['|'] = 
        Follow_re_link_level[')'] = 1;
    
    Follow_re_closure_level['$'] = Follow_re_closure_level['|'] = 
        Follow_re_closure_level[')'] = 1;

    Follow_re_top_level['$'] = Follow_re_top_level['|'] = Follow_re_top_level['*'] =
        Follow_re_closure_level['+'] = Follow_re_closure_level['?'] = 
            Follow_re_closure_level[')'] = 1;

    /* 非特殊字符的下一个符号可能是:  $, |, *, +, ?, ) */
    Follow_non_special['$'] = Follow_non_special['|'] = Follow_non_special['*'] =
        Follow_non_special['+'] = Follow_non_special['?'] = 
            Follow_non_special[')'] = 1;

    /* 转义字符的下一个符号可能是:   $, |, *, +, ?, ) 
     */
    Follow_re_top_level['$'] = Follow_re_top_level['|'] = Follow_re_top_level['*'] =
        Follow_re_closure_level['+'] = Follow_re_closure_level['?'] = 
            Follow_re_closure_level[')'] = 1;

    /* 转义字符的下一个符号可能是:   $, |, *, +, ?, ) 
     */
    Follow_character_range['$'] = Follow_character_range['|'] = 
        Follow_character_range['*'] = Follow_character_range['+'] = 
            Follow_character_range['?'] = Follow_character_range[')'] = 1;

    Follow_closure_op['$'] = Follow_closure_op['|'] = Follow_closure_op[')'] = 1;
    Follow_range[']'] = 1;
}
