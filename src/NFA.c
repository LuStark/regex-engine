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
};


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
    appendOutEdge (nfa->start, e);

    setStatusID (nfa->end, 2);
    appendOutEdge (nfa->end, e);

    ensureFinalStatus (nfa->end);

    /* 初始化nfa的Edge数组 */
    nfa->edgeArray = Array_new (1, sizeOfEdge());
    Array_put (nfa->edgeArray, 0, e);

    /* 初始化nfa的Status状态数组 */
    nfa->statusArray = Array_new (2, sizeOfStatus());
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
    nfa->edgeArray = Array_new (0, sizeOfEdge());

    /* 初始化nfa的Status状态数组 */
    nfa->statusArray = Array_new (1, sizeOfStatus());
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
    
    appendOutEdge(fromS, e);
    appendInEdge(toS, e);

}

NFA CopyNFA (NFA nfa)
{
    int i, id, n_status ;
    NFA     copyNFA ;
    Status  fromS, toS, s1, s2;
    Edge    e, e_temp;

    copyNFA = malloc (sizeof (NFA));
    assert (copyNFA);

    /* 首先，复制nfa状态数组中所有状态 */
    n_status = Array_length (nfa->statusArray);
    copyNFA -> statusArray = Array_copy (nfa->statusArray, n_status);
    
    for (i=0; i<n_status; i++)
    {
        //copyNFA->statusArray[i]->InEdges = NULL;
        //copyNFA->statusArray[i]->OutEdges = NULL;
        
        //copyNFA->statusArray[i]->InEdges = Array_new(0, sizeOfEdge());
        //copyNFA->statusArray[i]->OutEdges = Array_new(0, sizeOfEdge());
    }

    /* 第二步，复制所有边(除了指向前后Status的储存单元 */
    copyNFA -> edgeArray = Array_copy (nfa->edgeArray,
                                    Array_length(nfa->edgeArray));

    /* 第三部，针对edgeArray的每一条边，生成每个Status的InEdges和OutEdges */
    for (i=0; i < Array_length (nfa->edgeArray); i++)
    {
        e = (Edge)Array_get (nfa->edgeArray, i);
        fromS   = getfromStatus (e);
        toS     = gettoStatus (e);

        e = Array_get (copyNFA->edgeArray, i);
        
        id = getStatusID (fromS);
        s1 = Array_get (copyNFA->statusArray, id);
        appendOutEdge (s1, e);
        
        id = getStatusID (toS);
        s2 = Array_get (copyNFA->statusArray, id);
        appendInEdge (s2, e);
        
        setFromToStatus (e, s1, s2);
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
    toNFAcopy = CopyNFA (toNFA);

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
    combined_NFA->statusArray = Array_new (n1+n2, sizeOfStatus());
    /* 直接进行值拷贝 */
    Array_copy_from_range (combined_NFA->statusArray, 0, frontNFAcopy->statusArray, 0, n1);

    Array_copy_from_range (combined_NFA->statusArray, n1, toNFAcopy->statusArray, 0, n2);

    /* 合并 frontNFAcopy 和 toNFAcopy 的所有状态 */
    n1 = Array_length (frontNFAcopy->edgeArray);
    n2 = Array_length (toNFAcopy->edgeArray);
    combined_NFA->edgeArray = Array_new (n1+n2+1, sizeOfEdge()); 

    Array_copy_from_range (combined_NFA->edgeArray, 0, frontNFAcopy->edgeArray, 0, n1);
    Array_put (combined_NFA->edgeArray, n1, bridge);
    Array_copy_from_range (combined_NFA->edgeArray, n1+1, toNFAcopy->edgeArray, 0, n2);

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

    combineNFA = malloc (sizeof (struct NFA));
    assert(combineNFA);
    combineNFA->start = start ;
    combineNFA->end   = end ;
    ensureFinalStatus (combineNFA->end);

    /* 构造合成nfa的状态数组 */
    n1 = Array_length (nfa1copy->statusArray);
    n2 = Array_length (nfa2copy->statusArray);

    combineNFA->statusArray = Array_new(n1+n2+2, sizeOfStatus());
    
    /* 初始化nfa状态数组的第一个状态 */
    Array_put (combineNFA->statusArray, 0, start);
    
    Array_copy_from_range (combineNFA->statusArray, 1, nfa1copy->statusArray, 0, n1);
    Array_copy_from_range (combineNFA->statusArray, n1+1, nfa2copy->statusArray, 0, n2);
    
    Array_put (combineNFA->statusArray, n1+n2+1, end);
    

    // 储存边集合 
    n1 = Array_length (nfa1copy->edgeArray);
    n2 = Array_length (nfa2copy->edgeArray);
    combineNFA->edgeArray = Array_new (n1+n2+4, sizeOfEdge());

    Array_put (combineNFA->edgeArray, 0, e1_start);
    Array_put (combineNFA->edgeArray, 1, e2_start);
    
    Array_copy_from_range (combineNFA->edgeArray, 2, nfa1copy->edgeArray, 0, n1);
    Array_copy_from_range (combineNFA->edgeArray, n1+2, nfa2copy->edgeArray, 0, n2);

    Array_put (combineNFA->edgeArray, n1+n2+2, e1_end);
    Array_put (combineNFA->edgeArray, n1+n2+3, e2_end);
    
    /* ID 问题 */
    n = Array_length(combineNFA->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get (combineNFA->statusArray, i);
        setStatusID (s, i);
    }

    return combineNFA ;
}

NFA Closure(NFA nfa)
{
//    Edge *start_to_nfastart, *nfaEnd_to_nfastart, *nfaEnd_to_end, *start_to_end;
    // 把nfa的起始状态记为ns, nfa的终止状态: ne
    // 新nfa的起始状态 s, 新nfa终止状态 e,

    Edge    s2ns, ne2ns, ne2e, s2e;

    Status  start, end ;
    Status  temp, s;

    NFA     nfacopy;
    NFA     newnfa;

    int i, numE, numS, n;
    numE = numS = 0 ;

    nfacopy = CopyNFA(nfa);
   
    newnfa = malloc (sizeof (struct NFA));
    assert (newnfa);

    /* 构造边数组 */
    // newnfa->numOfEdges = nfacopy -> numOfEdges + 4;
    // newnfa -> edgeArray = malloc( sizeof(Edge*) * newnfa->numOfEdges );
    numE = Array_length (nfacopy->edgeArray);
    newnfa->edgeArray = Array_new (numE+4, sizeOfEdge());
    
    /* 构造状态数组 */
    // 得到nfa状态个数
    numS = Array_length (nfacopy->statusArray);
    newnfa->statusArray = Array_new (numS+2,sizeOfStatus());

    /* 构造初始，终止状态, 并加入到Status数组上 */
    newnfa->start = allocStatus();
    Array_put (newnfa->statusArray, 0, newnfa->start);
    Array_copy_from_range (newnfa->statusArray, 1, nfacopy->statusArray, 0, numS);
    Array_put (newnfa->statusArray, numS+1, newnfa->end);

    /* 初始状态到原nfa的start */
    s2ns = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (newnfa->start, nfacopy->start, s2ns);
    Array_put (newnfa->edgeArray, 0, s2ns);

    Array_copy_from_range (newnfa->edgeArray, 1, nfacopy->edgeArray, 0, numE);

    /* 原nfa的end -> 原nfa的start */
    ne2ns = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (nfacopy->end, nfacopy->start, ne2ns);
    Array_put (newnfa->edgeArray, numE+1, ne2ns);

    /* 原nfa的end状态 -> 新nfa的end状态 */
    ne2e  = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (nfacopy->end, newnfa->end, ne2e);
    Array_put (newnfa->edgeArray, numE+2, ne2e);
    
    /* 新nfa的start -> 新nfa的end */
    s2e = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (newnfa->start, newnfa->end, s2e);
    Array_put (newnfa->edgeArray, numE+3, s2e);

    /* ID 问题 */
    n = Array_length(newnfa->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get (newnfa->statusArray, i);
        setStatusID (s, i);
    }

    return newnfa;
}

NFA Option (NFA nfa)
{
//    Edge *start_to_nfastart, *nfaEnd_to_nfastart, *nfaEnd_to_end, *start_to_end;
    // 把nfa的起始状态记为ns, nfa的终止状态: ne
    // 新nfa的起始状态 s, 新nfa终止状态 e,

    Edge    s2ns, ne2ns, ne2e, s2e;

    Status  start, end ;
    Status  temp, s;

    NFA     nfacopy;
    NFA     newnfa;

    int i, numE, numS, n;
    numE = numS = 0 ;

    nfacopy = CopyNFA(nfa);
   
    newnfa = malloc (sizeof (struct NFA));
    assert (newnfa);

    /* 构造边数组 */
    // newnfa->numOfEdges = nfacopy -> numOfEdges + 4;
    // newnfa -> edgeArray = malloc( sizeof(Edge*) * newnfa->numOfEdges );
    numE = Array_length (nfacopy->edgeArray);
    newnfa->edgeArray = Array_new (numE+3, sizeOfEdge());
    
    /* 构造状态数组 */
    // 得到nfa状态个数
    numS = Array_length (nfacopy->statusArray);
    newnfa->statusArray = Array_new (numS+2,sizeOfStatus());

    /* 构造初始，终止状态, 并加入到Status数组上 */
    newnfa->start = allocStatus();
    Array_put (newnfa->statusArray, 0, newnfa->start);
    Array_copy_from_range (newnfa->statusArray, 1, nfacopy->statusArray, 0, numS);
    Array_put (newnfa->statusArray, numS+1, newnfa->end);

    /* 初始状态到原nfa的start */
    s2ns = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (newnfa->start, nfacopy->start, s2ns);
    Array_put (newnfa->edgeArray, 0, s2ns);

    Array_copy_from_range (newnfa->edgeArray, 1, nfacopy->edgeArray, 0, numE);

    /* 原nfa的end状态 -> 新nfa的end状态 */
    ne2e  = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (nfacopy->end, newnfa->end, ne2e);
    Array_put (newnfa->edgeArray, numE+1, ne2e);
    
    /* 新nfa的start -> 新nfa的end */
    s2e = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge (newnfa->start, newnfa->end, s2e);
    Array_put (newnfa->edgeArray, numE+2, ne2e);

    /* ID 问题 */
    n = Array_length(newnfa->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get (newnfa->statusArray, i);
        setStatusID (s, i);
    }

    return newnfa;
}

NFA Repeat_atleast_one( NFA nfa )
{
    NFA nfa1, nfa2, newnfa ;

    nfa1 = CopyNFA( nfa ) ;
    nfa2 = Closure( nfa ) ;

    newnfa = Link( nfa1, nfa2 ) ;
    return newnfa;
}



void RangeCopy(
                Range *toRangeArray,
                Range *fromRangeArray,
                int size
            )
{
    /*
    int i ;
    if( size <= 0 )
        return ;
    toRangeArray = malloc( sizeof( Range ) * size ) ;
    isNullPointer( toRangeArray ) ;

    for( i = 0; i < size; i++ )
        toRangeArray[ i ] = fromRangeArray[ i ];
    */
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

void generateCondSet( NFA nfa, Edge *CondSet,  int *size )
{
    /*
    Condition   cache[ MAXLEN_REGEXP ];
    int     i, j, cacheSize = 0 ;
    int     is_repeat_condition;

    for( i = 0; i < nfa->numOfEdges; i++ )
    {
        if( nfa->edgeArray[i]->hasEpsilon == false )
        {
            // 遍历cache集合中所有条件，观察有无重复的条件 
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
    *size = cacheSize ;

    for( i = 0; i < *size; i++ )
      CondSet[i] = cache[i];
    */
}

bool isMatchCondition (Condition c, wchar_t a)
{
    /*
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
    
    if( c->numOfInMatchRange > 0 || c->inMatchContent )
        return true;

    return false;
    */
}

bool
EqualCondition (Condition c1, Condition c2)
{
    return true;
}

int
Status_Transfer_Under_Condition(const Status status, Condition cond)
{
    /*
    int i;
    for(i=0; i < status->numOfOutEdges; i++ )
      if(EqualCondition( status->OutEdges[i], cond ))
        return status->OutEdges[i]->to_Status->ID;
    return -1;
    */
}

void freeNFA (NFA nfa)
{

}


void printNFA (NFA nfa)
{
    int i;
    Edge e;
    Status s1, s2;
    for (i=0; i<Array_length(nfa->edgeArray); i++)
    {
        e = Array_get (nfa->edgeArray, i);
        s1 = getfromStatus (e);
        s2 = gettoStatus (e);
        wprintf(L"(%d, %d, ", getStatusID(s1), getStatusID(s2));
        printEdge(e);
        wprintf(L")");
        if (isFinalStatus(s2))
        {
            wprintf(L"   (F)");
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
}
