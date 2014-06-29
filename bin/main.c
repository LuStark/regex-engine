#include "global.h"

xchar_t *RegExp[ MAXNUM_REGEXP ];

Index    current_RegExp_Index ;
Index    current_RegExpArray_Index ;

int  numOfRegExps;

int main( )
{
    int i, j ;
    xchar_t     newRegExp[100] ;
    NFA *nfa, *nfa2, *nfa3, *dfa;
    StatusSet   s1,s2,S[8];
    Condition   CondSet[100];
    int     sizeofCondSet;
//    HashTable_For_Wrap  HT;

//    Scan( "reg_exam1.txt" );
    //nfa = CharRange( RegExp[0], 0 );
//    for( i=0; i<1; i++ )
    wprintf(L"Regular Expression detecting: \n");
    wprintf(L">>>  ");
    while( wscanf(L"%ls",newRegExp) != EOF )
    {
        nfa = RegexpToNFA( newRegExp ) ;
        if( !nfa ){
            exit(1);
        }
        wprintf(L"NFA graph:\n");
        printNFA( nfa );
        //getchar();
    
        dfa = Subset_Construct( nfa );
        wprintf(L"DFA graph:\n");
        printNFA( dfa );

        wprintf(L">>>  ");
    }
    /* 无法识别的正则表达式: */
    /* b(a) */

}
