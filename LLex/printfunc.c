#include "NFA.h"
#include "global.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>


void printStatus( Status *s )
{
    printf("Status(%d)\n",s->ID);
}

bool isMatchInEdge( xchar_t c, Edge *e )
{
    int i ;
    if( e->matchContent )
    {
        for( i = 0; e->matchContent[i] != L'\0'; i++ )
            if( c == e->matchContent[i] ) return true;
    }
    if( e->inMatchContent )
    {
        for( i = 0; e->inMatchContent[i] != L'\0'; i++ )
            if( c == e->inMatchContent[i] ) return false;
    }

    for( i = 0; i < e->numOfMatchRange; i++ )
        if( e->matchRange[i].from <= c && c <= e->matchRange[i].to )
            return true;

    for( i = 0; i < e->numOfInMatchRange; i++ )
        if( e->inMatchRange[i].from <= c && c <= e->inMatchRange[i].to )
            return false;
    
    /* 若numOfInMatchRange大于0, 或者 inMatchContent不为空,
     * 表明c不在[^...]的范围内，可以通行 */
    if( e->numOfInMatchRange > 0 || e->inMatchContent )
        return true;

    return false;
}

Status* nextStatus( Status *currStatus ,xchar_t recvChar )
{
    Edge *e;
    int i ;
    for( i = 0; i < currStatus->numOfOutEdges; i++ )
    {
        e = currStatus->OutEdges[i];
        if( e==NULL ) continue;
        if( isMatchInEdge( recvChar, e ) )
            return e->to_Status ;
    }
    return NULL ;
}

void printEdge( Edge *e )
{
    int i ;
    if( e->matchContent && wcslen( e-> matchContent ) > 0 )
    {
        wprintf(L"matchContent: ");
        wprintf(L"%ls  ", e->matchContent );
    }
    if( e->inMatchContent && wcslen( e-> inMatchContent ) > 0 )
    {
        wprintf(L"inMatchContent: ");
        wprintf(L"%ls  ",e->inMatchContent);
    }
    if( e->numOfMatchRange > 0 ){
        wprintf(L"matchRange: ");
        for( i = 0; i< e->numOfMatchRange; i++ )
        {
            putwchar( e->matchRange[i].from );
            putwchar( L'-' );
            putwchar( e->matchRange[i].to );
            putwchar( ' ' );
            putwchar( ' ' );
        }
    }
    if( e->numOfInMatchRange > 0 ){
        wprintf(L"InMatchRange: ");
        for( i = 0; i< e->numOfInMatchRange; i++ )
        {
            putwchar( e->inMatchRange[i].from );
            putwchar( L'-' );
            putwchar( e->inMatchRange[i].to );
            putwchar( L' ' );
            putwchar( L' ' );
        }
    }

    if( e->hasEpsilon == true ){
        wprintf(L"epsilon tranformation. ");
    }
    if( e->AllowedAnyChar == true ){
        wprintf(L"any char pass. \n");
    }
}

void printNFA( NFA *nfa )
{
    int i;
    Edge *e ;
    if( nfa->start->FinalStatus ==true )
        wprintf(L"0:\t(F)\n");
    for( i = 0; i < nfa->numOfEdges; i++ )
    {
        e = nfa -> edgeArray[ i ];
        wprintf(L"%d -> %d:   ",e->from_Status->ID, e->to_Status->ID);
        printEdge( e ) ;
        if( e->to_Status->FinalStatus == true )
            wprintf(L"  (F)");
        wprintf(L"\n");
    }
    wprintf(L"\n");
}
