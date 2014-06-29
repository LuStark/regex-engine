#include "global.h"
#include "NFA.h"
#include "hash_for_wrap.h"
#include "set_status_wrap.h"

xchar_t *RegExp[ MAXNUM_REGEXP ];

Index    current_RegExp_Index ;
Index    current_RegExpArray_Index ;

int  numOfRegExps;


int main()
{
    int i, j ;
    xchar_t *newRegExp ;
    NFA *nfa, *nfa2, *nfa3, *dfa;
    StatusSet   *s1,*s2,S[8];
    Condition   CondSet[100];
    int     sizeofCondSet;
    
    HashTable_For_Wrap  *HT;
    Hash_LinkNode       p;
    Set_Status_Wrap     Wrap; 

    HT  =   malloc( sizeof(HashTable_For_Wrap) );

    Scan( "reg_exam1.txt" );
    //nfa = CharRange( RegExp[0], 0 );
    nfa = RegexpToNFA( RegExp[0] ) ;
    wprintf(L"NFA graph:\n");
    //printNFA( nfa );

    for( i=0; i<8; i++)
      S[i]=getEpsilonClosure(nfa,0);
    Wrap.statusSet  = S[0];
    Wrap.status     = nfa->start;
    
    HashTable_Insert( HT, Wrap );
    /* find the statusSet the same as S[0] */
    p  =   Find( HT, S[0]);
    printStatusSet( p->wrap.statusSet );
}
