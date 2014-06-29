#include "NFA.h"
#include "status_set.h"
#include "constant.h"
#include "cmp.h"

bool
isEqual_StatusSet( const StatusSet s1, 
                   const StatusSet s2 )
{
    int i;
    if( s1.eps_closure.size != s2.eps_closure.size )
      return false ;
    /* 逐个元素对比，认为生成顺序一致 */
    for( i = 0; i < s1.eps_closure.size; i++ )
      if( s1.eps_closure.Item[i] != s2.eps_closure.Item[i] )
        return false;
    return true;
}

StatusSet
getEpsilonClosure( NFA *nfa, int statusID )
{
    int     Queue[ MAXLEN_REGEXP ] ;
    int     i, front, tail ;
    int     parentID ;
    int     buffer[ 1000 ], size ;
    Status *status;
    Edge    *e;

    size = front = tail = 0 ;
    // 初始ID为 0
    Queue[ tail++ ] = statusID ;
    // BFS
    while( front!=tail )
    {
        parentID = Queue[front++] ;
        buffer[ size++ ] = parentID ;
        status  =  nfa -> Status[ parentID ] ;

        for( i = 0; i < status -> numOfOutEdges; i++ )
        {
            e = status -> OutEdges[ i ] ;
            if( e->hasEpsilon == true )
                Queue[ tail++ ] = e -> to_Status -> ID;
        }
    }
    StatusSet s;
    initialize( &s.StatusID );
    append(&s.StatusID, statusID );
    initialize( &s.eps_closure );
    for( i = 0; i < size; i++ )
        append( &s.eps_closure, buffer[ i ]);

    return s;
}

StatusSet     
StatusSet_Transfer( 
                    NFA *nfa, 
                    SingleStatusSet Sets[], 
                    StatusSet currentSet, 
                    Condition cond 
                )
{
    int     i,  status_ID, t_ID ;
    int     transfer_status_ID;     // 记录转换后状态的ID
    int     Table[ MAXNUM_STATES ]; //状态转换表

    /* newSet_epsilon 是 newSet集合下的所有集合的epsilon闭包的并集 */
    MultiStatusSet  newSet ;

    initialize( &newSet.StatusID );
    initialize( &newSet.eps_closure );

    for( i = 0; i < currentSet.eps_closure.size; i++ )
    {
        /* 针对当前状态（currentSet）的epsilon闭包中每一个元素 */
        status_ID = currentSet.eps_closure.Item[ i ] ;
        
        /* 检测NFA中此状态ID 能否在cond条件下转换成另一个状态 */
        if( (transfer_status_ID = 
                    Status_Transfer_Under_Condition
                    ( nfa->Status[ status_ID ], cond ))!= -1 )
        {
            /* 闭包中状态id 符合转换条件，添加到新集合的状态向量里 */
            Table[ status_ID ] = transfer_status_ID ;
            append( &newSet.StatusID, status_ID ) ;
        }
    }
     
    /* 得到newSet的所有StatusID以后，计算所有状态的闭包的并集 */
    /* 比如: StatusID = {1,2,6}  eps_closure= eps(1) | eps(2) | eps */
    for( i = 0; i < newSet.StatusID.size; i++ )
    {
        // UnionBuffer储存并集 
        status_ID = newSet.StatusID.Item[i];
        t_ID  = Table[ status_ID ];
        newSet.eps_closure = UnionVector( newSet.eps_closure, Sets[t_ID].eps_closure );
    }
    return newSet;
}

void printStatusSet( StatusSet s )
{
    int     i;
    wprintf(L"StatusID: ");
    for( i=0; i<s.StatusID.size; i++)
      wprintf(L"%d ",s.StatusID.Item[i]);
    wprintf(L"\n");
    wprintf(L"eps_closure: ");
    for( i=0; i<s.eps_closure.size; i++)
      wprintf(L"%d ",s.eps_closure.Item[i]);
    wprintf(L"\n");
}


void LinkList_Insert( StatusSet_listHeader *L, int item )
{
    StatusSet_linkNode      newLinkNode;
    if( (*L)==NULL )
    {
        (*L) = malloc( sizeof( struct node ) );
        (*L)->next = (*L)->front = NULL;
        (*L) -> StatusID = item; 
    }
    else{
        newLinkNode = malloc( sizeof( struct node ) );
        newLinkNode -> StatusID = item;

        (*L) -> front = newLinkNode;
        newLinkNode -> next  = (*L);
        (*L) = newLinkNode;
    }
}

void connect( StatusSet_listHeader *L, StatusSet_linkNode *p )
{
    if( (*L) == NULL )
    {
        (*L) = (*p);
    }else{
        (*p) -> next    = (*L);
        (*L) -> front   = (*p);
        (*L) = (*p);
    }
}



