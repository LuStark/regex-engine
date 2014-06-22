#include "global.h"

xchar_t *RegExp[ MAXNUM_REGEXP ];

Index    current_RegExp_Index ;
Index    current_RegExpArray_Index ;

int  numOfRegExps;

int main()
{
    int i, j ;
    xchar_t *newRegExp ;
    NFA *nfa, *nfa2, *nfa3, *dfa;
    StatusSet   s1,s2,S[8];
    Condition   CondSet[100];
    int     sizeofCondSet;
    HashTable_For_Wrap  HT;

    Scan( "reg_exam1.txt" );

    for(i=0; i<1; i++)
    {
        newRegExp   =   PreprocessExpression(RegExp[i]);
        if( newRegExp != NULL ){
            wprintf(L"%ls\n",newRegExp);
        }else{
            wprintf(L"illegal expression!\n");
        }
    }

}
