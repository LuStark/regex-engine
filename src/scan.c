#include "scan.h"
#include "global.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "typedef.h"


void Scan( char *filename )
{
    xchar_t c ;
    FILE *f ;
    int  i, ignoreCounts, lenOfCurrRegExp ;

    f = fopen( filename, "r" );
    if ( f==NULL ){
        return ;
    }

    ignoreCounts = 0 ;
    numOfRegExps = 0 ;

    lenOfCurrRegExp = 0 ;
    RegExp[ 0 ] = malloc( sizeof( xchar_t ) * MAXLEN_REGEXP );

    while( (c=fgetwc( f ))!=EOF )
    {
        if( c == L' ' || c == L'\t' )
        {
            ignoreCounts ++ ;
            continue ;
        }
        if( c == L'\n' )
        {
            if ( lenOfCurrRegExp == 0 )
                continue ;
            RegExp[ numOfRegExps ][ lenOfCurrRegExp ] = L'\0';
            RegExp[ ++numOfRegExps ] = malloc( sizeof( xchar_t ) * MAXLEN_REGEXP ); 

            if ( !RegExp[ numOfRegExps ] ){
                printf("内存空间不足\n");
            }

            lenOfCurrRegExp = 0;
            continue ;
        }
        RegExp[ numOfRegExps ][ lenOfCurrRegExp++ ] = c ;
    }
}

