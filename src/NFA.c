#include "NFA.h"
#include "constant.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>
#include "Array.h"

typedef struct NFA *NFA;

struct NFA
{
    Status      start, end;
    Array_T     edgeArray;
    Array_T     statusArray;
}


NFA CreateSingleNFA (wchar_t c)
{
    NFA  nfa ;
    
    Edge e ;

    nfa = malloc (sizeof (struct NFA));
    assert(nfa); 
    
    /* 初始化唯一的两个状态点 */
    nfa -> start = allocStatus();
    nfa -> end   = allocStatus();
    
    /* 初始化唯一的边 */
    e = allocEdge();
    setFromToStatus (e, nfa->start, nfa->end);
    addCharacter (e, c);

    setStatusID (nfa->start, 1);
    // 创建长度为１，元素大小为Edge结构体的动态数组.
    nfa->OutEdges = Array_new (1, sizeof(struct Edge)); 
    Array_put (nfa->OutEdges, 0, e);

    setStatusID (nfa->end, 2);
    // 创建长度为１，元素大小为Edge结构体的动态数组.
    nfa->InEdges = Array_new (1, sizeof(struct Edge));
    Array_put (nfa->InEdges, 0, e);

    ensureFinalStatus (nfa->end);

    /* 初始化nfa的Edge数组 */
    nfa->edgeArray = Array_new (1, sizeof(struct Edge));
    Array_put (nfa->edgeArray, 0, e);
    /* 初始化nfa的Status状态数组 */
    nfa->statusArray = Array_new (2, sizeof(struct Status));
    Array_put (nfa->statusArray, 0, nfa->start);
    Array_put (nfa->statusArray, 1, nfa->end);

    return nfa ;
}

NFA CreateNFA_without_edge()
{
    NFA  nfa ;
    
    nfa = malloc (sizeof (struct NFA));
    assert(nfa); 
    
    /* 初始化唯一的两个状态点 */
    nfa->start = nfa->end = allocStatus();
    
    setStatusID (nfa->start, 1);

    ensureFinalStatus (nfa->start);

    /* 初始化nfa的Edge数组, 长度为0 */
    nfa->edgeArray = Array_new (0, sizeof(struct Edge));

    /* 初始化nfa的Status状态数组 */
    nfa->statusArray = Array_new (1, sizeof(struct Status));
    Array_put (nfa->statusArray, 0, nfa->start);

    return nfa ;
}

void 
linkTwoStatus_by_AnEdge( Status fromS , 
                         Status toS ,
                         Edge   e
                        )
{
    assert(fromS);
    assert(toS);
    assert(e);
    setFromToStatus (e, fromS, toS);
    
    Array_append (fromS->OutEdges, e);
    Array_append (toS->InEdges, e);

}

NFA CopyNFA (NFA nfa)
{
    int i, id, n_status ;
    NFA     copyNFA ;
    Status  fromS, toS;

    copyNFA = malloc (sizeof (NFA));
    assert (copyNFA);

    /* 首先，复制nfa状态数组中所有状态 */
    n_status = Array_length (nfa->statusArray);
    copyNFA -> statusArray = Array_copy (nfa->statusArray, n_status);
    
    for (i=0; i<n_status; i++)
    {
        copyNFA->statusArray[i]->InEdges = NULL;
        copyNFA->statusArray[i]->OutEdges = NULL;
        copyNFA->statusArray[i]->InEdges = Array_new(0, sizeof(struct Edge));
        copyNFA->statusArray[i]->OutEdges = Array_new(0, sizeof(struct Edge));
    }

    /* 第二步，复制所有边(除了指向前后Status的储存单元 */
    copyNFA -> edgeArray = Array_copy (nfa->edgeArray,
                                    Array_length(nfa->edgeArray));

    /* 第三部，针对edgeArray的每一条边，生成每个Status的InEdges和OutEdges */
    for (i=0; i < Array_length (nfa->edgeArray); i++)
    {
        e = (Edge)Array_get (nfa->edgeArray);
        fromS   = e -> from;
        toS     = e -> to;

        Array_append (
                    copyNFA->statusArray[fromS->ID]->OutEdges, 
                    copyNFA->edgeArray[i]
                    );
        Array_append (
                    copyNFA->statusArray[toS->ID]->InEdges,
                    copyNFA->edgeArray[i]
                    );
        setFromToStatus (copyNFA->edgeArray[i], 
                    copyNFA->statusArray[fromS->ID],
                    copyNFA->statusArray[toS->ID]
                    );
    }

    /* 最后，更新start 和 end */
    copyNFA -> start = Array_get (copyNFA->statusArray, 0); 
    copyNFA -> end   = Array_get (copyNFA->statusArray, n_status-1); 
    ensureFinalStatus (copyNFA->end);
    
    return copyNFA;
}

NFA Link (NFA frontNFA, NFA toNFA )
{
    int i, j, numStatus ;
    int n1, n2, n;
    NFA combined_NFA;
    NFA frontNFAcopy, toNFAcopy ;
    Edge e, bridge;
    Status s;

    /* 连结两个NFA图的边是epsilon edge */
    bridge = allocEpsilonEdge();
    
    frontNFAcopy = CopyNFA (frontNFA);
    toNFAcopy = CopyNFA (tailNFA);

    cancelFinalStatus (frontNFAcopy->end);
    cancelFinalStatus (toNFAcopy->end);

    linkTwoStatus_by_AnEdge (frontNFAcopy->end, toNFAcopy->start, e);

    combined_NFA = malloc (sizeof (struct NFA));
    assert(combined_NFA);

    combined_NFA->start = frontNFAcopy -> start ;
    combined_NFA->end = toNFAcopy -> end ;
    ensureFinalStatus (combined_NFA->end);

    /* 合并 frontNFAcopy 和 toNFAcopy 的所有状态 */
    n1 = Array_length (frontNFAcopy->statusArray);
    n2 = Array_length (toNFAcopy->statusArray);
    combined_NFA->statusArray = Array_new (n1+n2, sizeof(struct Status));
    /* 直接进行值拷贝 */
    for (i=0; i<n1; i++)
    {
        s = Array_get (frontNFAcopy->statusArray, i);
        Array_put (combined_NFA->statusArray, i, s); 
    }
    for (i=0; i<n2; i++)
    {
        s = Array_get (toNFAcopy->statusArray, i);
        Array_put (combined_NFA->statusArray, i+n1, s); 
    }

    
    /* 合并 frontNFAcopy 和 toNFAcopy 的所有状态 */
    n1 = Array_length (frontNFAcopy->edgeArray);
    n2 = Array_length (toNFAcopy->edgeArray);
    combined_NFA->edgeArray = Array_new (n1+n2+1, sizeof(struct Edge)); 

    for (i=0; i<n1; i++)
    {
        e = Array_get (frontNFAcopy->edgeArray, i);
        Array_put (combined_NFA->edgeArray, i, e);
    }
    Array_put (combined_NFA->edgeArray, n1, bridge);
    for (i=0; i<n2; i++)
    {
        e = Array_get (toNFAcopy->edgeArray, i);
        Array_put (combined_NFA->edgeArray, i, e);
    }

    /* ID 问题 */
    n = Array_length(combined_NFA->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get (combined_NFA->statusArray, i);
        setStatusID (s, i);
    }

    return combined_NFA ;
}

NFA Union (NFA nfa1, NFA nfa2 )
{
    int i, j, n1, n2, n;
    NFA     combineNFA, nfa1copy, nfa2copy ;
    Status  start, end;
    Status  s;

    Edge    e1_start, e2_start;
    Edge    e1_end, e2_end;
    Edge    e;

    nfa1copy = CopyNFA (nfa1);
    nfa2copy = CopyNFA (nfa2);
    cancelFinalStatus (nfa1copy->end);
    cancelFinalStatus (nfa2copy->end);

    start = allocStatus(); 
    end   = allocStatus();

    e1_start = allocEpsilonEdge();
    e2_start = allocEpsilonEdge();
    e1_end = allocEpsilonEdge();
    e2_end = allocEpsilonEdge();

    linkTwoStatus_by_AnEdge (start, nfa1copy->start, e1_start);
    linkTwoStatus_by_AnEdge (start, nfa2copy->start, e2_start);
    
    linkTwoStatus_by_AnEdge (nfa1copy->end, end, e1_end);
    linkTwoStatus_by_AnEdge (nfa2copy->end, end, e2_end);

    combineNFA = malloc (sizeof (NFA));
    combineNFA->start = start ;
    combineNFA->end   = end ;
    ensureFinalStatus (combineNFA->end);

    /* 构造合成nfa的状态数组 */
    n1 = Array_length (nfa1copy->numOfStatus);
    n2 = Array_length (nfa2copy->numOfStatus);

    combineNFA->statusArray = Array_new(n1+n2+2, sizeof(struct Status));
    
    /* 初始化nfa状态数组的第一个状态 */
    Array_put (combineNFA->statusArray, 0, start);
    
    for (i=0; i<n1; i++)
    {
        s = Array_get (nfa1copy->statusArray, i);
        Array_put (combineNFA->statusArray, i+1, s);
    }
    for (i=0; i<n2; i++)
    {
        s = Array_get (nfa2copy->statusArray, i);
        Array_put (combineNFA->statusArray, n1+1+i, s);
    }
    Array_put (combineNFA->statusArray, n1+n2+1, end);
    

    // 储存边集合 
    n1 = Array_length (nfa1copy->edgeArray);
    n2 = Array_length (nfa2copy->edgeArray);
    combineNFA->edgeArray = Array_new (n1+n2+4)

    Array_put (combineNFA->edgeArray, 0, e1_start);
    Array_put (combineNFA->edgeArray, 1, e2_start);
    
    for (i=0; i<n1; i++)
    {
        e = Array_get (nfa1copy->edgeArray, i);
        Array_put (combineNFA->edgeArray, i+2);
    }
    for (i=0; i<n2; i++)
    {
        e = Array_get (nfa2copy->edgeArray, i);
        Array_put (combineNFA->edgeArray, n1+i+2);
    }

    Array_put (combineNFA->edgeArray, n1+n2, e1_end);
    Array_put (combineNFA->edgeArray, n1+n2+1, e2_end);
    
    /* ID 问题 */
    n = Array_length(combined_NFA->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get (combined_NFA->statusArray, i);
        setStatusID (s, i);
    }

    return combineNFA ;
}

NFA Closure( NFA *nfa )
{
    Edge *start_to_nfastart, *nfaEnd_to_nfastart, *nfaEnd_to_end, *start_to_end;
    Status *start, *end ;
    NFA *nfacopy;
    NFA *newnfa;

    int i, numE, numS;
    numE = numS = 0 ;

    nfacopy = CopyNFA(nfa);
   
    newnfa = malloc( sizeof( NFA ) );
    isNullPointer( newnfa );

    /* 构造边数组 */
    newnfa->numOfEdges = nfacopy -> numOfEdges + 4;
    newnfa -> edgeArray = malloc( sizeof(Edge*) * newnfa->numOfEdges );
    isNullPointer( newnfa->edgeArray );
    /* 构造状态数组 */
    newnfa->numOfStatus = nfacopy -> numOfStatus + 2 ;
    newnfa -> Status = malloc( sizeof(Status*) * newnfa->numOfStatus );
    isNullPointer( newnfa->Status );
    /* 构造初始，终止状态, 并加入到Status数组上 */
    newnfa -> start = allocStatus();
    newnfa -> Status[ numS++ ] = newnfa -> start ;
    newnfa -> end = allocStatus();
    newnfa -> Status[ newnfa->numOfStatus -1 ] = newnfa->end;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy -> numOfStatus; i++ )
        newnfa->Status[ numS ++ ] = nfacopy->Status[ i ] ;


    /* 初始状态到原nfa的start */
    start_to_nfastart = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, nfacopy->start, start_to_nfastart );
    newnfa -> edgeArray[ numE++ ] = start_to_nfastart ;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy->numOfEdges; i++ )
        newnfa -> edgeArray[ numE++ ] = nfacopy -> edgeArray[ i ];

    /* 原nfa的end -> 原nfa的start */
    nfaEnd_to_nfastart = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( nfacopy->end, nfacopy->start, nfaEnd_to_nfastart );
    newnfa -> edgeArray[ numE++ ] = nfaEnd_to_nfastart ;

    /* 原nfa的end状态 -> 新nfa的end状态 */
    nfaEnd_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( nfacopy->end, newnfa->end, nfaEnd_to_end ) ;
    newnfa -> edgeArray[ numE++ ] = nfaEnd_to_end ;
    
    /* 新nfa的start -> 新nfa的end */
    start_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, newnfa->end, start_to_end );
    newnfa -> edgeArray[ numE++ ] = start_to_end ;

    if( numE != newnfa->numOfEdges ){
        printf("numE != numOfEdges ");
        while(1){}
    }

    for( i = 0; i < newnfa->numOfStatus; i++ ){
        newnfa -> Status[ i ]->ID = i;
    }
    return newnfa ;
}

NFA
*Option( NFA *nfa )
{
    Edge *start_to_nfastart, *nfaEnd_to_end, *start_to_end;
    Status *start, *end ;
    NFA *nfacopy;
    NFA *newnfa;

    int i, numE, numS;
    numE = numS = 0 ;

    nfacopy = CopyNFA(nfa);
   
    newnfa = malloc( sizeof( NFA ) );
    isNullPointer( newnfa );

    /* 构造边数组 */
    newnfa->numOfEdges = nfacopy -> numOfEdges + 3;
    newnfa -> edgeArray = malloc( sizeof(Edge*) * newnfa->numOfEdges );
    isNullPointer( newnfa->edgeArray );
    /* 构造状态数组 */
    newnfa->numOfStatus = nfacopy -> numOfStatus + 2 ;
    newnfa -> Status = malloc( sizeof(Status*) * newnfa->numOfStatus );
    isNullPointer( newnfa->Status );
    /* 构造初始，终止状态, 并加入到Status数组上 */
    newnfa -> start = allocStatus();
    newnfa -> Status[ numS++ ] = newnfa -> start ;
    newnfa -> end = allocStatus();
    newnfa -> Status[ newnfa->numOfStatus -1 ] = newnfa->end;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy -> numOfStatus; i++ )
        newnfa->Status[ numS ++ ] = nfacopy->Status[ i ] ;


    /* 初始状态到原nfa的start */
    start_to_nfastart = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, nfacopy->start, start_to_nfastart );
    newnfa -> edgeArray[ numE++ ] = start_to_nfastart ;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy->numOfEdges; i++ )
        newnfa -> edgeArray[ numE++ ] = nfacopy -> edgeArray[ i ];

    /* 原nfa的end状态 -> 新nfa的end状态 */
    nfaEnd_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( nfacopy->end, newnfa->end, nfaEnd_to_end ) ;
    newnfa -> edgeArray[ numE++ ] = nfaEnd_to_end ;
    
    /* 新nfa的start -> 新nfa的end */
    start_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, newnfa->end, start_to_end );
    newnfa -> edgeArray[ numE++ ] = start_to_end ;

    if( numE != newnfa->numOfEdges ){
        printf("numE != numOfEdges ");
        while(1){}
    }

    for( i = 0; i < newnfa->numOfStatus; i++ ){
        newnfa -> Status[ i ]->ID = i;
    }
    return newnfa ;
}

NFA
*Repeat_atleast_one( NFA *nfa )
{
    NFA *nfa1, *nfa2, *newnfa ;

    nfa1 = CopyNFA( nfa ) ;
    nfa2 = Closure( nfa ) ;

    newnfa = Link( nfa1, nfa2 ) ;
    return newnfa;
}




Edge *CopyEdge( Edge *e )
{
    Edge *newedge ;
    newedge = malloc( sizeof( Edge ) );
    /* 值拷贝 */
    *newedge = *e ;
    newedge -> from_Status = newedge -> to_Status = NULL ;

    if( e->matchContent != NULL )
        wcscpy( newedge->matchContent, e->matchContent );

    if( e->inMatchContent != NULL )
        wcscpy( newedge -> inMatchContent, e -> inMatchContent ) ;
        
    RangeCopy( newedge -> matchRange , e->matchRange, e->numOfMatchRange) ;
    RangeCopy( newedge -> inMatchRange, e->inMatchRange, e->numOfInMatchRange ) ; 
    
    return newedge ;

}

void RangeCopy(
                Range *toRangeArray,
                Range *fromRangeArray,
                int size
            )
{
    int i ;
    if( size <= 0 )
        return ;
    toRangeArray = malloc( sizeof( Range ) * size ) ;
    isNullPointer( toRangeArray ) ;

    for( i = 0; i < size; i++ )
        toRangeArray[ i ] = fromRangeArray[ i ];
}


bool
itemInSet( int id, int Set[], int size )
{
    int i;
    for( i = 0; i < size; i++ )
        if( id == Set[i] )
            return true;
    return false;
}

void 
generateCondSet( NFA *nfa, Condition *CondSet,  int *size )
{
    Condition   cache[ MAXLEN_REGEXP ];
    int     i, j, cacheSize = 0 ;
    int     is_repeat_condition;

    for( i = 0; i < nfa->numOfEdges; i++ )
    {
        if( nfa->edgeArray[i]->hasEpsilon == false )
        {
            /* 遍历cache集合中所有条件，观察有无重复的条件 */
            is_repeat_condition = 0;
            for( j=0; j<cacheSize; j++ )
            {
                if( EqualCondition( nfa->edgeArray[i], cache[j] ))
                {
                    is_repeat_condition = 1;
                    break;
                }
            }
            if( is_repeat_condition == 0 )
                cache[ cacheSize++ ] = nfa -> edgeArray[i]; 
        }
    }
//    CondSet = malloc( sizeof(Condition) * cacheSize ) ;
//    isNullPointer( CondSet ) ;
    /* 返回条件集合的大小 */
    *size = cacheSize ;

    for( i = 0; i < *size; i++ )
      CondSet[i] = cache[i];

}

bool isMatchCondition( Condition c, wchar_t a )
{
    int i ;
    if( c->matchContent )
    {
        for( i = 0; c->matchContent[i] != L'\0'; i++ )
            if( a == c->matchContent[i] ) return true;
    }
    if( c->inMatchContent )
    {
        for( i = 0; c->inMatchContent[i] != L'\0'; i++ )
            if( a == c->inMatchContent[i] ) return false;
    }

    for( i = 0; i < c->numOfMatchRange; i++ )
        if( c->matchRange[i].from <= a && a <= c->matchRange[i].to )
            return true;

    for( i = 0; i < c->numOfInMatchRange; i++ )
        if( c->inMatchRange[i].from <= a && a <= c->inMatchRange[i].to )
            return false;
    
    /* 若numOfInMatchRange大于0, 或者 inMatchContent不为空,
     * 表明c不在[^...]的范围内，可以通行 */
    if( c->numOfInMatchRange > 0 || c->inMatchContent )
        return true;

    return false;
}

bool
EqualCondition( Condition c1, Condition c2 )
{
    wchar_t *a, *b;
    int     i ;

    if( c1->AllowedAnyChar != c2->AllowedAnyChar )
      return false;

    if( wstrcmp_allow_nullpointer( c1->matchContent,c2->matchContent )!=0 )
      return false;
    if( wstrcmp_allow_nullpointer( c1->inMatchContent,c2->inMatchContent)!=0 )
      return false;

    if( c1->numOfMatchRange != c2->numOfMatchRange )
      return false;

    for( i=0; i < c1->numOfMatchRange; i++ )
      if( !RangeEqual( c1->matchRange[i],c2->matchRange[i] ))
        return false;

    if( c1->numOfInMatchRange != c2->numOfInMatchRange )
      return false;
    
    for( i=0; i < c1->numOfInMatchRange; i++ )
      if( !RangeEqual( c1->inMatchRange[i],c2->inMatchRange[i] ))
        return false;

    return true;
}

int
Status_Transfer_Under_Condition(const Status *status, Condition cond)
{
    int i;
    for(i=0; i < status->numOfOutEdges; i++ )
      if(EqualCondition( status->OutEdges[i], cond ))
        return status->OutEdges[i]->to_Status->ID;
    return -1;
}

void
freeNFA( NFA *nfa )
{

}
