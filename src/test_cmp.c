#include "cmp.h"
#include <stdio.h>

main()
{
    wchar_t *A = L"abc";
    wchar_t *B = L"abc";

    if( wstrcmp_allow_nullpointer(A,B)==0 )
        printf("Equal!\n");
    else
        printf("Inequal!\n");
}
