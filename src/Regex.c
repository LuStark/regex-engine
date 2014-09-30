#include "Regex.h"
#include "NFA.h"
#include "typedef.h"
#include "automaton.h"
#include <stdlib.h>

/* 为DFA创建一个状态转换表 T[i][j], 状态值i在字符j下转换到的状态 */
DFATable
makeUpDFATable (DFA dfa)
{
    int i, j;
    Edge    e;
    /* 针对DFA的每条边e, 其上的非匹配字符 记录在此数组中 */
    bool    isInMatchChar[128];
    char    c;
    Range   range;
    int     fromID, toID;

    DFATable    T;
    T   =  malloc (sizeof(int*) * dfa->numOfStatus);
    assert(T);

    for( i=0; i < Array_length(dfa->statusArray); i++ )
    {
        T[i] = malloc (sizeof(int) * 128);
        assert(T[i]);
        for (j=0; j<128; j++)  T[i][j] = -1;
    }

    for (i=0; i < Array_length(dfa->edgeArray); i++)
    {
        e = Array_get(dfa->edgeArray, i);
        fromID  =   getStatusID(getfromStatus(e));
        toID    =   getStatusID(gettoStatus(e));

        /* 判断终态 */
        if (e->AllowedAnyChar == true)
        {
            for( j=0; j<128; j++)
              T[fromID][j] = toID;
        }
        if (e->matchContent != NULL)
        {
            /* 循环遍历matchContent每个字符然后填表 */
            for (j=0; e->matchContent[j]!='\0'; j++)
            {
                c = e->matchContent[j]; 
                T[ fromID ][ c ] = toID;
            }
        }
        if (e->inMatchContent != NULL)
        {
            for (j=0; e->inMatchContent[j]!='\0'; j++)
            {
                c = e->inMatchContent[j];
                /* 不匹配的项都映射为1 */
                isInMatchChar[c] = true;
            }
            for ( j=0; j<128; j++ ) 
            {
                if (isInMatchChar[j] == false) 
                    T[fromID][j] = toID;
                isInMatchChar[j] = false;
            }
        }
        if( e->numOfMatchRange > 0 )
        {
            for (j=0; j < e->numOfMatchRange; j++)
            {
                range = e->matchRange[j];
                for( c = range.from; c <= range.to; c++ )
                    T[ fromID ][ c ] = toID;
            }
        }
        if( e->numOfInMatchRange > 0 )
        {
            for( j=0; j < e->numOfInMatchRange ; j++ )
            {
                range = e->inMatchRange[j];
                for( c = range.from; c<=range.to; c++ )
                    isInMatchChar[ c ] = true;
            }
            for( c=0; c<128; c++ )
              if( isInMatchChar[c] == false )
                T[ fromID ][ c ] = toID;
        }

    }

    return T; 
}

bool
Recognition( DFAEntity entity, xchar_t *normal )
{    
    int i;
    int length = wcslen( normal );
    xchar_t     c;

    i = 0;
    while( (c=(*normal++)) != '\0' )
    {
        if( entity.T[i][c] == -1 )
          return false;
        else
          i = entity.T[i][c];  /* i有可能是自己 */
    }
    /* 最后到达的状态是i */
    if( entity.dfa->Status[i]->FinalStatus )
        return true;

    /* 暂时无法预见的错误 */
    return false;
}
