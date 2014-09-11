#include "NFA.h"
#include "constant.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <assert.h>
#include "Array.h"
#include "automaton.h"

#include "Edge.h"
#include "Status.h"



static void adjustStatusID(NFA nfa)
{
    int i,n;
    Status s;

    n = Array_length(nfa->statusArray);
    for (i=0; i<n; i++)
    {
        s = Array_get(nfa->statusArray, i);
        setStatusID(s, i);
    }
    nfa->start = Array_get(nfa->statusArray, 0);
    nfa->end = Array_get(nfa->statusArray, n-1);

    ensureFinalStatus (nfa->end);
}

void linkTwoStatusInNFA (NFA nfa, int from, int to, int e)
{
    assert(nfa);
    
    Status  fromS, toS;
    Edge    bridge;
    Array_T   Edges;

    fromS = Array_get (nfa->statusArray, from);
    toS   = Array_get (nfa->statusArray, to);
    
    bridge = Array_get (nfa->edgeArray, e);
    
    assert(fromS);
    assert(toS);
    assert(bridge);
    setFromToStatus (bridge, fromS, toS);
    
    if (!getOutEdges(fromS))
        initOutEdges(fromS);
    appendOutEdge(fromS, e);

    if (!getInEdges(toS))
        initInEdges(toS);
    appendInEdge(toS, e);

}


NFA CreateNFA (int n, int e)
{
    assert(n>0);
    assert(e>=0);
    
    NFA p;
    p = malloc (sizeof (struct Automaton));
    assert (p);
    if (e>0)
        p->edgeArray = allocEdgeArray(e);
    p->statusArray = allocStatusArray(n); 
    if (n==1)
        p->start = p->end = Array_get(p->statusArray, 0);
    else
    {
        p->start = Array_get(p->statusArray,0);
        p->end = Array_get(p->statusArray,n-1);
    }
    adjustStatusID(p);

    return p;
}

NFA CreateSingleNFA (wchar_t c)
{
    NFA  nfa;
    
    Edge e;

    nfa = malloc (sizeof (struct Automaton));
    assert(nfa); 
    
    /* 初始化nfa的Edge数组 */
    nfa->edgeArray = allocEdgeArray(1); 

    /* 初始化nfa的Status状态数组 */
    nfa->statusArray = allocStatusArray(2);
   
    e = Array_get(nfa->edgeArray, 0);
    clearBits(e);
    addCharacter (e, c);
    
    linkTwoStatusInNFA(nfa,0,1,0);

    adjustStatusID(nfa);
    ensureFinalStatus (nfa->end);

    return nfa ;
}

NFA CreateNFA_without_edge()
{
    NFA  nfa ;
    
    nfa = malloc (sizeof (struct Automaton));
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

Status getStartStatus (NFA nfa)
{
    assert(nfa);
    assert(nfa->start);
    return nfa->start;
}
Status getEndStatus (NFA nfa)
{
    assert(nfa);
    assert(nfa->end);
    return nfa->end;
}

Edge getEdge (NFA nfa, int i)
{
    assert(nfa);
    assert(nfa->edgeArray);
    assert(i<Array_length(nfa->edgeArray) && i>=0);

    return Array_get (nfa->edgeArray, i);
    
}


NFA CopyNFA (NFA nfa)
{
    int i, id, n;
    NFA     copyNFA ;
    Status  fromS, toS, s1, s2;
    Edge    e, e_temp;

    copyNFA = malloc (sizeof (NFA));
    assert (copyNFA);

    /* 首先，复制nfa状态数组中所有状态 */
    n = Array_length (nfa->statusArray);
    copyNFA -> statusArray = Array_new (n, sizeOfStatus());
    Array_copy_from_range (copyNFA->statusArray, 0, nfa->statusArray, 0, n);

    /* 第二步，复制所有边(除了指向前后Status的储存单元 */
    n = Array_length (nfa->edgeArray);
    copyNFA -> edgeArray = Array_new (n, sizeOfEdge());
    Array_copy_from_range (copyNFA->edgeArray, 0, nfa->edgeArray, 0, n);

    /* 第三部，针对edgeArray的每一条边，生成每个Status的InEdges和OutEdges */
    for (i=0; i < Array_length (nfa->edgeArray); i++)
    {
        e = (Edge)Array_get (nfa->edgeArray, i);
        fromS   = getfromStatus (e);
        toS     = gettoStatus (e);

        e = Array_get (copyNFA->edgeArray, i);
        
        id = getStatusID (fromS);
        s1 = Array_get (copyNFA->statusArray, id);
        appendOutEdge (s1, i);
        
        id = getStatusID (toS);
        s2 = Array_get (copyNFA->statusArray, id);
        appendInEdge (s2, i);
        
        setFromToStatus (e, s1, s2);
    }

    /* 最后，更新start 和 end */
    n = Array_length (copyNFA->statusArray);
    copyNFA -> start = Array_get (copyNFA->statusArray, 0); 
    copyNFA -> end   = Array_get (copyNFA->statusArray, n-1); 
    ensureFinalStatus (copyNFA->end);
    
    return copyNFA;
}

// 根据nfa中图结构，为copyNFA构建一样的图结构，其中
// copyNFA中的顶点下标从s_offset开始计数，边下标从e_offset开始计数
static void adjustStatusEdges (NFA copyNFA, NFA nfa, int s_offset, int e_offset)
{
    int i, id, id2;
    Edge e, e_temp;
    Status s1, s2, fromS, toS;
    for (i=0; i < Array_length (nfa->edgeArray); i++)
    {
        e_temp  = Array_get (nfa->edgeArray, i);
        e = Array_get (copyNFA->edgeArray, i+e_offset);
        copyEdge_without_Status (e, e_temp);
        
        id = getStatusID (getfromStatus (e_temp));
        s1 = Array_get (copyNFA->statusArray, id+s_offset);

        id2 = getStatusID (gettoStatus (e_temp));
        s2 = Array_get (copyNFA->statusArray, id2+s_offset);
        if (id==id2)
        {
            wprintf(L"error!!!\n");
        }

        linkTwoStatusInNFA (copyNFA, id+s_offset, id2+s_offset, i+e_offset);
        //linkTwoStatus_by_AnEdge (s1, s2, e);
    }
}

NFA Link (NFA frontNFA, NFA toNFA )
{
    int i, j, numStatus ;
    int n1, n2, n;
    NFA combined_NFA;
    Edge bridge;
    Status s, s1, s2;

    
    combined_NFA = malloc (sizeof (struct Automaton));
    assert(combined_NFA);

    /* 构造combined_NFA所有状态 */
    n = Array_length (frontNFA->statusArray) + Array_length (toNFA->statusArray);
    
    combined_NFA->statusArray = allocStatusArray(n);
    
    adjustStatusID (combined_NFA);
    combined_NFA->start = Array_get(combined_NFA->statusArray, 0);
    combined_NFA->end = Array_get(combined_NFA->statusArray, n-1);
    ensureFinalStatus(combined_NFA->end);
    
    /* combined_NFA边集合 <=> 两个nfa以及连接它们的边ε*/
    n = Array_length(frontNFA->edgeArray) + Array_length(toNFA->edgeArray) + 1;
    
    combined_NFA->edgeArray = allocEdgeArray(n);
    
    adjustStatusEdges(combined_NFA, frontNFA, 0, 0);
    
    n1 = Array_length (frontNFA->statusArray);
    n2 = Array_length (toNFA->statusArray);
    /* 连结两个NFA图的边是epsilon edge */
    bridge = Array_get(combined_NFA->edgeArray, Array_length(frontNFA->edgeArray)); 
    setEpsilon (bridge);
    // 提取对应于frontNFA状态的最后一个状态
    s1 = Array_get (combined_NFA->statusArray, n1-1);
    s2 = Array_get (combined_NFA->statusArray, n1);
    linkTwoStatusInNFA (combined_NFA, n1-1, n1, Array_length(frontNFA->edgeArray));
    // linkTwoStatus_by_AnEdge (s1, s2, bridge);
     

    /* 根据已有的NFA, 确认combined_NFA中边和点之间的关系 */
    /* 已边数: frontNFA所有边以及bridge, 所以边计数从n开始*/
    n = Array_length (frontNFA->edgeArray) + 1;
    adjustStatusEdges (combined_NFA, toNFA, n1, n);

    freeNFA(frontNFA);
    freeNFA(toNFA);

    return combined_NFA ;
}

/*
 *  nfa1 G: n1×e1    nfa2 G: n2×e2
 *  以下是 nfa1 和 nfa2的联合 (记为combined_NFA)
 *

 *  (0号边)            ------    (e1+e2+2)号边
 *    ————————————>  |  nfa1  | —————————————
      |                ------               ｜
      |                                     ｖ
    -----                                 -----
  | start | (0号顶点)                    | end | (n1+n2+1) 号顶点
    -----                                 -----
      |                                   　^
      |                ------        　     ｜
      ————————————>  |  nfa2  | ————————————
    (1号边)            ------    (e1+e2+3)号边

    
    边顺序:     0号 
                1号  
                nfa1中的e1条边:    2, 3, ...  e1
                nfa2中e2条边:      e1+2, e1+3, ..., e1+e2+1
                (e1+e2+2)号边
                (e1+e2+3)号边

    顶点顺序:   start:  0号,  
                nfa1中n1个顶点:     1, 2, 3, ..., n1
                nfa2中n2个顶点:     n1+1, n1+2, ... n1+n2  
                end :   n1+n2+1


    以下算法关于combined_NFA的顶点边编排顺序如上所述
 */

NFA Union (NFA nfa1, NFA nfa2 )
{
    int i, j, n1, n2, n, e1, e2;
    NFA     combined_NFA;
    Status  s;

    Edge    e;

    /* 得到两个NFA 边(Edge)数 和 顶点(Status)数 */
    n1 = Array_length (nfa1->statusArray);
    n2 = Array_length (nfa2->statusArray);
    e1 = Array_length (nfa1->edgeArray);
    e2 = Array_length (nfa2->edgeArray);

    n = n1 + n2 + 2;

    combined_NFA = malloc (sizeof (struct Automaton));
    assert(combined_NFA);

    combined_NFA->statusArray = allocStatusArray(n);


    /* 构造边集合, 即两个NFA的边数加上4条ε边 */
    n = e1 + e2 + 4;
    combined_NFA->edgeArray = allocEdgeArray(n); 

    /* 提取0号边 */
    e = Array_get (combined_NFA->edgeArray, 0);
    setEpsilon (e);
    linkTwoStatusInNFA (combined_NFA, 0, 1, 0);
    
    adjustStatusEdges (combined_NFA, nfa1, 1, 2);
     
    e = Array_get (combined_NFA->edgeArray, 1);
    setEpsilon (e);
    linkTwoStatusInNFA (combined_NFA, 0, n1+1, 1);

    adjustStatusEdges (combined_NFA, nfa2, n1+1, e1+2);

    setEpsilon (Array_get (combined_NFA->edgeArray, e1+e2+2));
    linkTwoStatusInNFA (combined_NFA, n1, n1+n2+1, e1+e2+2);

    setEpsilon (Array_get (combined_NFA->edgeArray, e1+e2+3));
    linkTwoStatusInNFA (combined_NFA, n1+n2, n1+n2+1, e1+e2+3);

    adjustStatusID (combined_NFA);
    ensureFinalStatus (combined_NFA->end);

    freeNFA(nfa1);
    freeNFA(nfa2);

    return combined_NFA ;
}

/*
 *  nfa1 G: n×e
 *  以下是 求nfa闭包
 *
                       e+2 号边
                 —————————————————————
                ｜                   ｜
                ｜                   ｜       
 *  (0号边)      —>    ------   —————— 
 *    ————————————>  |  nfa  | —————————————————
      |                ------        e+3号边    ｜
      |                                         ｖ
    -----                                     -----
  | start | (0号顶点)                        | end | (n+1) 号顶点
    -----                                     -----
　　  |                                         ｜
    　———————————————————————————————————————————
                       1号边
    
    边顺序:     0号 
                1号  
                nfa中的e条边:    2, 3, ...  e
                e+2号边
                (e+3)号边

    顶点顺序:   start:  0号,  
                nfa中n个顶点:     1, 2, 3, ..., n
                end :   n+1


 */
NFA Closure(NFA nfa)
{
    NFA     newnfa;

    int n, e;

    newnfa = malloc (sizeof (struct Automaton));
    assert (newnfa);

    /* 构造边数组 */
    e = Array_length (nfa->edgeArray);
    newnfa->edgeArray = allocEdgeArray (e+4);
  
    // 得到nfa状态个数
    n = Array_length (nfa->statusArray);
    newnfa->statusArray = allocStatusArray (n+2);

    /* 提取0号边 */
    setEpsilon (Array_get (newnfa->edgeArray, 0));
    linkTwoStatusInNFA (newnfa, 0, 1, 0);

    /* 提取1号边 */
    setEpsilon (Array_get (newnfa->edgeArray, 1));
    linkTwoStatusInNFA (newnfa, 0, n+1, 1);

    /* 在newnfa中构造与nfa对应一样的图结构 */
    adjustStatusEdges (newnfa, nfa, 1, 2);

    /* 提取e+2号边 */
    setEpsilon (Array_get (newnfa->edgeArray, e+2));
    linkTwoStatusInNFA (newnfa, n, 1, e+2);

    /* 提取e+3号边 */
    setEpsilon (Array_get (newnfa->edgeArray, e+3));
    linkTwoStatusInNFA (newnfa, n, n+1, e+3);
    
    adjustStatusID (newnfa); 

    freeNFA(nfa);
    
    return newnfa;
}

/*
 *  nfa1 G: n×e
 *  以下是 求nfa 0-1闭包
 *
 *  (0号边)            ------ 
 *    ————————————>  |  nfa  | —————————————————
      |                ------        e+2号边    ｜
      |                                         ｖ
    -----                                     -----
  | start | (0号顶点)                        | end | (n+1) 号顶点
    -----                                     -----
　　  |                                         ｜
    　———————————————————————————————————————————
                       1号边

 */
NFA Option (NFA nfa)
{
    NFA     newnfa;

    int n, e;

    newnfa = malloc (sizeof (struct Automaton));
    assert (newnfa);

    /* 构造边数组 */
    e = Array_length (nfa->edgeArray);
    newnfa->edgeArray = allocEdgeArray (e+3);
    
    // 得到nfa状态个数
    n = Array_length (nfa->statusArray);
    newnfa->statusArray = allocStatusArray (n+2);

    /* 提取0号边 */
    setEpsilon (Array_get (newnfa->edgeArray, 0));
    linkTwoStatusInNFA (newnfa, 0, 1, 0);

    /* 提取1号边 */
    setEpsilon (Array_get (newnfa->edgeArray, 1));
    linkTwoStatusInNFA (newnfa, 0, n+1, 1);

    /* 在newnfa中构造与nfa对应一样的图结构 */
    adjustStatusEdges (newnfa, nfa, 1, 2);

    /* 提取e+2号边 */
    setEpsilon (Array_get (newnfa->edgeArray, e+2));
    linkTwoStatusInNFA (newnfa, n, n+1, e+2);

    adjustStatusID (newnfa); 

    freeNFA(nfa);
    
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
    Edge    e;
    int i;
    assert(nfa);
    
    for (i=0; i<Array_length(nfa->edgeArray); i++)
    {
        e = Array_get(nfa->edgeArray,i);
        unreferenceStatus (e);
        clearBits(e);
    }

    for (i=0; i<Array_length(nfa->statusArray); i++)
        freeEdgesInStatus (Array_get(nfa->statusArray, i));

    if (nfa->statusArray)
        Array_free (&nfa->statusArray);
    if (nfa->edgeArray)
        Array_free (&nfa->edgeArray);
    free (nfa);
    nfa=NULL;
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
