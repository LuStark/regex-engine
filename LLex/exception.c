#include "NFA.h"
#include <stdlib.h>
#include <stdio.h>

void isNullPointer( void *p )
{
    if( p == NULL )
    {
        printf("Pointer p is a null pointer\n");
        exit(1);
    }
}

void Error(char *str){
    printf("%s\n",str);
    getchar();
}
