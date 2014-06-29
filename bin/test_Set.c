#include "Set.h"
#include <stdio.h>

int main()
{
    int i;
    Set s1;
    initSet(&s1);
    
    for( i=0; i<99; i++ )
        s1.append(&s1,i);

    for( i=0; i<s1.num; i++ )
        printf("%d\n",s1.a[i]);
}


