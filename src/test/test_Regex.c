#include "../global.h"
#include "../LLex.h"
#include "../typedef.h"
#include "../DFAEntity.h"
#include <stdio.h>
#include <string.h>

xchar_t *RegExp[ MAXNUM_REGEXP ];

Index    current_RegExp_Index ;
Index    current_RegExpArray_Index ;

int  numOfRegExps;

int main( )
{
    int i, j, state ;
    xchar_t     newRegExp[100] ;
    char        c, buffer[200];
    NFA *nfa, *nfa2, *nfa3, *dfa;
    DFAEntity   dfaentity;

    StatusSet   s1,s2,S[8];
    Condition   CondSet[100];
    int     sizeofCondSet;
    wprintf(L"Input Regular Expression: \n");
    wprintf(L">>>  ");

    while( wscanf(L"%ls",newRegExp) != EOF )
    {
        nfa = RegexpToNFA( newRegExp ) ;
        if( !nfa ){
            exit(1);
        }
    
        dfa = Subset_Construct( nfa );

        printNFA( dfa );

        dfaentity.dfa = dfa;
        dfaentity.numOfStatus = dfa->numOfStatus;
        dfaentity.T = makeUpDFATable( dfa );

        i = state = 0;
        wprintf(L">>>  ");
        
        //FILE *f = fopen("regex.txt","r");
    
        /*
        if( !f ){
            wprintf(L"Can not read in the file.\n");
            exit(1);
        }

        while( (c=fgetc(f)) != EOF )
        {
            if( c=='\n' )
            {
                c=fgetc(f);
                if( c!=EOF )
                    ungetc(c,f);
                else
                    break;
            }
            state = dfaentity.T[state][c];
            if( state == -1 )
            {
                wprintf(L"Fail...\n");
                break;
            }
        }
        if( state == -1 )
        {
            break;
        }
        if( dfaentity.dfa->Status[state]->FinalStatus )
        {
            wprintf(L"Match succeed!\n");
            break;
        }else{
            wprintf(L"Fail...\n");
            break;
        }
        fclose(f);
        */
    }
}
