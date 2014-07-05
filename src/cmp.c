#include "cmp.h"
#include <stdio.h>

int     
wstrcmp_allow_nullpointer(wchar_t *a,wchar_t *b)
{
    /* if both null pointer, regard as same string */
    if( a==NULL && b==NULL )
      return 0;
    /* if a is NULL but b is not, regard the bigger string is b */
    else if( a==NULL )
      return -1;
    else if( b==NULL )
      return 1;
    else
    {
        return wcscmp(a,b);
    }
}
