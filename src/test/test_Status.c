#include "NFA.h"
#include "typedef.h"
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

int main( )
{
    Status s1, s2, s3, s4, s5, *currStatus ;
    Edge *e1, *e2, *e3, *e4, *e5, *e6, *e7;
    wint_t c;
    s1.ID = 1; s2.ID = 2; s3.ID = 3; s4.ID = 4;
    s5.ID = 5; 
    e1 = malloc( sizeof(Edge) ); e2 = malloc(sizeof(Edge));
    e3 = malloc( sizeof(Edge) ); e4 = malloc( sizeof(Edge) );
    e5 = malloc( sizeof(Edge) ); e6 = malloc( sizeof(Edge) );
    e7 = malloc( sizeof(Edge) );

    {
    // e1 configure.
    e1->matchContent = malloc( sizeof( xchar_t ) * 1 );
    e1->matchContent[0] = L'/';
    e1->from_Statu = &s1;  e1->to_Statu = &s2;
    }
    {
    // e2 configure.
    e2->matchContent = malloc( sizeof( xchar_t ) * 1 );
    e2->matchContent[0] = L'*';
    e2->from_Statu = &s2;  e2->to_Statu = &s3;
    }
    {
    // e3 configure.
    e3 -> inMatchContent = malloc( sizeof( xchar_t ) * 1 );
    e3 -> inMatchContent[0] = L'*';
    e3 -> from_Statu = &s3 ;  e3->to_Statu = &s3 ;
    }
    {
    // e4 configure.
    e4->matchContent = malloc( sizeof( xchar_t ) * 1 );
    e4->matchContent[0] = L'*';
    e4->from_Statu = &s3;  e4->to_Statu = &s4;
    }
    {
    // e5 configure.
    e5->matchContent = malloc( sizeof( xchar_t ) * 1 );
    e5->matchContent[0] = L'*';
    e5->from_Statu = &s4;  e5->to_Statu = &s4;
    }
    {
    // e6 configure.
    e6->inMatchContent = malloc( sizeof( xchar_t ) * 2 );
    e6->inMatchContent[0] = L'*';
    e6->inMatchContent[1] = L'/';
    e6->from_Statu = &s4;  e6->to_Statu = &s3;
    }
    {
    // e7 configure.
    e7->matchContent = malloc( sizeof( xchar_t ) * 1 );
    e7->matchContent[0] = L'/';
    e7->from_Statu = &s4;  e7->to_Statu = &s5;
    }

    // s1 configure.
    {
    s1.numOfOutEdges = 1 ;
    s1.OutEdges = malloc( sizeof(Edge*) * s1.numOfOutEdges );
    s1.OutEdges[0] = e1;
    }
    // s2 configure.
    {
    s2.numOfOutEdges = 1 ;
    s2.numOfInEdges = 1 ;
    s2.OutEdges = malloc( sizeof(Edge*) * s2.numOfOutEdges );
    s2.InEdges  = malloc( sizeof(Edge*) * s2.numOfInEdges  );
    s2.OutEdges[0] = e2; 
    s2.InEdges[0] = e1;
    }
    // s3 configure.
    {
    s3.numOfOutEdges = 2 ;
    s3.numOfInEdges = 3 ;
    s3.OutEdges = malloc( sizeof(Edge*) * s3.numOfOutEdges );
    s3.InEdges  = malloc( sizeof(Edge*) * s3.numOfInEdges  );
    s3.OutEdges[0] = e3; s3.OutEdges[1] = e4;
    s3.InEdges[0] = e2; s3.InEdges[1] = e3; s3.InEdges[2] = e6;
    }
    // s4 configure.
    {
    s4.numOfOutEdges = 3 ;
    s4.numOfInEdges = 2 ;
    s4.OutEdges = malloc( sizeof(Edge*) * s4.numOfOutEdges );
    s4.InEdges  = malloc( sizeof(Edge*) * s4.numOfInEdges  );
    s4.OutEdges[0] = e5; s4.OutEdges[1] = e6; s4.OutEdges[2] = e7;
    s4.InEdges[0] = e4; s4.InEdges[1] = e5; 
    }
    // s5 configure.
    {
    s5.numOfInEdges = 1 ;
    s5.InEdges  = malloc( sizeof(Edge*) * s5.numOfInEdges  );
    s5.InEdges[0] = e7; 
    }

    currStatus = &s1 ;
    while( (c=getwchar())!=EOF )
    {
        if( c=='\n' ) continue;
        currStatus = nextStatus( currStatus,c ) ;
        if( currStatus != NULL ) {
            printStatus( currStatus ) ;
        }
        else
        {
            printf("匹配失败，回到起点。\n");
            currStatus = &s1;
        }
    }
}
