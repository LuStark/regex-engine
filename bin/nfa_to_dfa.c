#include "nfa_to_dfa.h"
#include "NFA.h"
#include "queue.h"
#include "status_set.h"
#include "constant.h"
#include "set_status_wrap.h"
#include "hash_for_wrap.h"
#include "Set.h"


static bool
has_Existed_Condition_In_Status(const Status *s1, const Status *s2, Condition cond)
{
    return (Status_Transfer_Under_Condition( s1,cond ) == s2->ID);
}

DFA
*Subset_Construct( NFA *nfa )
{
    /*  nfa中所有状态集合 */
    SingleStatusSet     Sets[ MAXNUM_STATES ];
    int     sizeOfSets ;

    /* 当前状态集合 和 它产生的子集（可能为空）分别记录 */
    StatusSet           currentSet, newSet ;
    /* 条件集合 */
    Condition           CondSet[ MAXNUM_EDGES ], cond;

    Edge        *edgeBuffer[ MAXNUM_EDGES ];
    
    /* 采用BFS构造DFA时，储存DFA状态的哈希表（即statusBuffer）
     * 和 储存状态集的哈希表（statusSetBuffer）是一一对应。
     * 例如： 当前状态集 currentSet    满足一定条件(cond)下可以转换到新状态集（newSet)，
     * <=>    当前状态   currentStatus 可以在这个条件(cond)下转换到新的状态（newStatus)
     * 那么,程序分别将newSet和newStatus储存到相应的哈希表，但采用同一个索引。 
     * 比如储存在第i号 
     */

    /* 哈希表采用头链接法 */
    Status      *statusBuffer[ MAXNUM_STATES ];
    StatusSet   *statusSetBuffer[ MAXNUM_STATES ];

    int         numOfConds;

    /* 记录当前的状态点，以及由这个状态点转换的下一个状态点 
     * 这些状态点和边是构成DFA的元素，其中状态点需要保存在
     * 队列中 */

    DFA                 *dfa;
    Edge                *newEdge;
    Status              *currentStatus, *newStatus ;

    /* Set_Status_Wrap 队列 */
    Set_Status_Wrap_Queue   Q;
    Set_Status_Wrap         current_wrap, new_wrap;

    /* hash table */
    HashTable_For_Wrap      *HT;
    Hash_LinkNode           searched_node;

    /* 循环变量 */
    int     i;
    /* 初始化队列参数 */
    Initialize_Queue( Q );

    /* 初始化哈希表 */
    HT  =   malloc( sizeof(HashTable_For_Wrap) );
    if( !HT )
        Error("memory exhausted!");

    /* 求出所有单个状态的ε闭包 */
    for( i=0; i < nfa->numOfStatus; i++ )
        Sets[i] = getEpsilonClosure( nfa, i );

    /* 求出NFA所有条件的集合 */
    generateCondSet( nfa, CondSet, &numOfConds );
    
    /* 初始化 DFA */
    dfa = malloc( sizeof(DFA) );
    dfa -> numOfStatus = dfa -> numOfEdges = 0;
    
    /* 检查是否分配内存失败 */
    isNullPointer( dfa );

    dfa->start  =   allocStatus();
    statusBuffer[ dfa->numOfStatus++ ] = dfa->start;

    /* 分别将初始状态的ε闭包、DFA的起始节点送进相应的队列 */
    current_wrap.statusSet  =   Sets[0];
    current_wrap.status     =   dfa->start;
    if( itemInSet( nfa->end->ID,Sets[0].eps_closure.Item,Sets[0].eps_closure.size )) 
        dfa->start->FinalStatus = true;
    Enqueue( Q, current_wrap );

    /* 执行BFS */
    while( !EmptyQueue(Q) )
    {
        current_wrap    = Dequeue(Q) ;

        currentSet      = current_wrap.statusSet ;
        currentStatus   = current_wrap.status ;
        
        /* 当前集合状态（currentSet），遍历NFA中所有条件（condition） */
        for( i=0; i<numOfConds; i++ )
        {
            cond        =   CondSet[i];
            newSet      =   StatusSet_Transfer( nfa, Sets, currentSet, cond);
            searched_node = Find( HT, newSet );

            if( searched_node != NULL )
            {
                //指向原来的状态 
                new_wrap    =   searched_node -> wrap;

                if( !has_Existed_Condition_In_Status( currentStatus, new_wrap.status, cond ))
                {
                    newEdge     =   CopyEdge( cond );
                    /* 连结到以前的某个状态 */
                    linkTwoStatus_by_AnEdge( currentStatus, new_wrap.status, newEdge );
                    edgeBuffer[ dfa->numOfEdges++ ] = newEdge;
                }
            }
            else if( newSet.StatusID.size>0 )
            {
                /* newSet的状态ID存在，说明currentSet在cond条件下转换成功 */
                newStatus   =   allocStatus();
                /* 如果nfa->end的ID是newSet的元素，说明newStatus是dfa的终点 */
                if( itemInSet( nfa->end->ID,newSet.eps_closure.Item,newSet.eps_closure.size)) 
                    newStatus->FinalStatus  =   true;

                newEdge     =   CopyEdge( cond );
                linkTwoStatus_by_AnEdge( currentStatus, newStatus, newEdge );

                /* 新创建的边和点分别进入相应的缓存数组中 */
                edgeBuffer[ dfa->numOfEdges++ ] = newEdge;
                statusBuffer[ dfa->numOfStatus++ ] = newStatus;

                /* 进队 */
                new_wrap.statusSet  =   newSet;
                new_wrap.status     =   newStatus;
    
                HashTable_Insert( HT, new_wrap );

                Enqueue( Q, new_wrap );
            }
        }
    }

    /* 关于dfa终点的定位，暂略 */
    //dfa->end = currentStatus;

    dfa -> edgeArray = malloc( sizeof(Edge*) * dfa->numOfEdges );
    isNullPointer( dfa->edgeArray );
    dfa -> Status = malloc( sizeof(Status*) * dfa->numOfStatus );
    isNullPointer( dfa->Status );

    for( i=0; i<dfa->numOfEdges; i++ )
      dfa->edgeArray[i] = edgeBuffer[i];
    for( i=0; i<dfa->numOfStatus; i++ )
      dfa->Status[i] = statusBuffer[i];

    for( i=0; i<dfa->numOfStatus; i++) dfa->Status[i]->ID = i;
   
//    wprintf(L"DFA graph(raw):\n");
//    printNFA( dfa );

//    dfa = MINI_Status_for_DFA( dfa );    
    dfa = MINIMUN_DFAStatus( dfa );

    return dfa;
}

static void
reassignID( DFA *dfa )
{
    Status  *Q[ MAXNUM_STATES ], *currentStatus, *newStatus;
    Edge    *e;
    int     front,rear;
    int     i, IDcount;
    Status  *newStatusBuffer[ MAXNUM_STATES ];

    front = rear = 0;
    Q[rear++] = dfa->start;
    
    for( i=0; i<dfa->numOfStatus; i++ ) 
      dfa->Status[i]->ID = -1;

    IDcount = 0;
    while( front!=rear )
    {
        currentStatus   =   Q[front++];
        if( currentStatus->ID == -1 )
        {
            for( i=0; i<currentStatus->numOfOutEdges; i++ )
            {
                e = currentStatus->OutEdges[i];
                newStatus = e->to_Status;
                Q[rear++] = newStatus;
            }
            currentStatus->ID = IDcount++;
            newStatusBuffer[ currentStatus->ID ] = currentStatus;
        }
    }
    for( i=0; i<dfa->numOfStatus; i++ )
      dfa->Status[i] = newStatusBuffer[i];    
}

static void
reassign_edge_order( DFA *dfa )
{
    int     i,j;
    Edge    *edgeBuffer[ MAXNUM_EDGES ];
    
    int     N = 0;

    for( i=0; i<dfa->numOfStatus; i++ )
      for( j=0; j<dfa->Status[i]->numOfOutEdges; j++ )
        edgeBuffer[ N++ ] = dfa->Status[i]->OutEdges[j];
    
    for( i=0; i<dfa->numOfEdges; i++ )
      dfa->edgeArray[i] = edgeBuffer[i];

}

/* 最小化DFA的方法： 
 *   假设当前存在K个集合（集合数组为S），每个集合的元素是原DFA的状态ID。
 *      记DFA状态x在特定条件c下的转换的函数为 T(x,c).  
 * ---------------------------------------------------------------
 * step0:   while( S中所有集合不再被分裂 )
 *          {
 * step1:       for each set Si In 集合数组S：
 *              {
 * step2:           for each condition c In 条件集：
 *                  {
 *                       
 * step3:               temp = T( Si[0],c )   注： Si[0]是指Si集合的首元素。
 *                      Let  S_1 = S_2 = {}.    S_1.add( Si[0] )
 *
 * step4:               for each xj in Si - { Si[0] }:
 *                      {
 * step5:                     yj = T( xj, c).
 *                            if yj == temp:  
 *                                  S_1.add( xj )
 *                            else:
 *                                  S_2.add( xj )
 *                      }
 * step6:               if S_2 != {}:
 *                          Si 被 S_1和S_2取代，即 S_1和S_2被插入数组S中。
 *                          这样，S被条件c拆分为 Si1和Si2两个集合。
 *                          这时候，就要重新回到step1。
 *                  } 
 *              }
 *          }
 * ---------------------------------------------------------------
 * 
 * 最后S中的每个集合中的元素都是等价状态。 这时候，我们需要构建
 *  一个集合状态表。 SetTable。定义如下：
 *
 *     SetTable[i][j] = Uset(k)   若集合Si中存在某个元素x, 满足 T(x，CondSet[j] ) = k.
                                其中CondSet是条件集合，Uset(k)是k在S中所在的集合
 *     SetTable[i][j] = -1.      集合Si所有元素都不满足 CondSet[j]的转换。
 */

DFA
*MINIMUN_DFAStatus( DFA *raw_dfa )
{
    Set     S[ MAXNUM_STATES ], S_1, S_2;
    int     numOfSets, SetsChanged;
    int     (*T)(const Status*,Condition);
    int     _i,i,j,k;
    int     x0, x, y0, y;

    DFA     *min_dfa;
    Condition   CondSet[ MAXNUM_EDGES ],cond;
    int     numOfConds;

    /* 状态ID->集合ID的并查集 */
    int     toSetID, UFSet[ MAXNUM_STATES ];
    bool    FinalStatus[ MAXNUM_STATES ];
    Edge    *newEdge, *edgeBuffer[MAXNUM_EDGES];
    int     numOfEdgeBuffer = 0;

    generateCondSet( raw_dfa, CondSet, &numOfConds);
    for( i=0; i<raw_dfa->numOfStatus; i++ )
    {
        if( raw_dfa->Status[i]->FinalStatus)
            FinalStatus[i] = true;
        else
            FinalStatus[i] = false;
    }

    /* 首先初始化两个集合，分别记录接受状态和非接受状态 */
    initSet( &S[0] );
    initSet( &S[1] );
    numOfSets   =   2;

    for( i=0; i<raw_dfa->numOfStatus; i++ )
    {
        if( !raw_dfa->Status[i]->FinalStatus )
            S[0].add( &S[0], i );
        else
            S[1].add( &S[1], i );
    }
    /* 因为DFA必然存在接受状态，但可能不存在非接受状态
     * 故须检测是否存在非接受状态 */
    if( S[0].size( &S[0] ) == 0 )
    {
        S[0]=S[1];
        numOfSets = 1;
    }

    /* T为状态转换的代理函数 */
    T   =   Status_Transfer_Under_Condition;
    SetsChanged = 1;
    while( SetsChanged )
    {
        SetsChanged = 0;
        for( i=0; i<numOfSets; i++ )
        {
            for( j=0; j<numOfConds; j++ )
            {
                cond = CondSet[j];
                /* x和y 分别是转换前的状态ID，转换后的状态ID */
                /* 提取S[i]中第1个状态ID */
                x0 = S[i].visit( &S[i],0 );

                y0 = T( raw_dfa->Status[x0], cond );
                initSet( &S_1 );
                initSet( &S_2 );
                S_1.add( &S_1,x0 );
                
                for( k=1; k<S[i].size( &S[i] ); k++ )
                {
                    x = S[i].visit( &S[i], k);
                    y = T( raw_dfa->Status[x], cond);
                    if( y==y0 )
                        S_1.add( &S_1,x );
                    else
                        S_2.add( &S_2,x );
                }
                if( S_2.size(&S_2) > 0 )
                {
                    SetsChanged = 1;
                    break;
                }
            }

            if( SetsChanged )
            {
                /* 将S[i]分解为 S_1 和 S_2 */
                /* 鉴于集合数量总是很少，故将S[i+1]以后所有集合后移 */
                for( _i=numOfSets-1; _i>i; _i-- )
                    S[_i+1] = S[_i];
                
                S[i] = S_1;
                S[i+1] = S_2;

                numOfSets++;
                break;
            }
        }
    }

    for( i=0; i<numOfSets; i++ )
    {
        for( j=0; j<S[i].size(&S[i]); j++ )
        {
            x = S[i].visit( &S[i], j );
            UFSet[ x ] = i;
        }
    }

    min_dfa = malloc(sizeof( DFA ));
    min_dfa -> numOfStatus = numOfSets;
    min_dfa -> Status = malloc( sizeof(Status*) * min_dfa->numOfStatus );
    isNullPointer( min_dfa->Status );

    for( i=0; i<min_dfa->numOfStatus; i++ )
    {
        min_dfa->Status[i] = allocStatus();
        min_dfa->Status[i]->ID = i;
    }

    for( i=0; i<numOfSets; i++ )
    {
        for( j=0; j<numOfConds; j++ )
        {
            cond = CondSet[j];
            x = S[i].visit( &S[i], 0 );
            y = T( raw_dfa->Status[x], cond );
            if( y!=-1 )
            {
                toSetID = UFSet[ y ];
                newEdge = CopyEdge( cond );
                edgeBuffer[ numOfEdgeBuffer++ ] = newEdge;
                linkTwoStatus_by_AnEdge(
                    min_dfa->Status[i],min_dfa->Status[ toSetID ], newEdge );
            }else{

            }
        }
    }

    min_dfa -> numOfEdges = numOfEdgeBuffer;
    min_dfa -> edgeArray = malloc( sizeof( Edge) * numOfEdgeBuffer );
    isNullPointer( min_dfa->edgeArray );
    for( i=0; i<numOfEdgeBuffer; i++ )
        min_dfa->edgeArray[i]=edgeBuffer[i];

    for( i=0; i<numOfSets; i++ )
    {
        for( j=0; j<S[i].size(&S[i]); j++ )
        {
            x = S[i].visit( &S[i],j );
            if( x == raw_dfa->start->ID )
                min_dfa->start = min_dfa->Status[i];
            if( FinalStatus[x] )
                min_dfa->Status[i]->FinalStatus = true;
        }
    }

    reassignID( min_dfa );
    reassign_edge_order( min_dfa );
    
    return min_dfa;
}


DFA
*MINI_Status_for_DFA( DFA *dfa )
{
    StatusSet_listHeader    S[ MAXNUM_STATES ], DFASet_header, newheader, p, q;
    int         numOfSets, numOfConds, numOfStatus;
    Condition   CondSet[ MAXNUM_EDGES ], cond;
    bool        FinalStatus[ MAXNUM_STATES ];
    
    int         i, j, k, currentID, setsChanged = 0;
    int         from_id, to_id, to_Set_ID;
    int         temp;
    /* 构建状态集ID 到 集合的映射 */
    int         Status_Set[ MAXNUM_STATES ];
    /* 瘦身版DFA */
    DFA         *slim_dfa;
    Edge        *newEdge, *edgeBuffer[MAXNUM_EDGES];
    int         numOfEdgeBuffer = 0;


    numOfStatus =   dfa->numOfStatus;
    generateCondSet( dfa, CondSet, &numOfConds );
    for( i=0; i<numOfStatus; i++ )
    {
        S[i] = NULL;
        if( dfa->Status[i]->FinalStatus == true )
            FinalStatus[i] = true;
        else
            FinalStatus[i] = false;
    }
    
    numOfSets   =   2;
    for( i=0; i<numOfStatus; i++ )
    {
        if( !FinalStatus[i] )
            LinkList_Insert( &S[0], i );
        else
            LinkList_Insert( &S[1], i );
    }
    
    /* 如果S[0]（非接受状态集合为空），S[1]覆盖S[0] */
    if( S[0]==NULL )
    {
        S[0] = S[1];
        S[1] = NULL;
        numOfSets = 1;
    }else if( S[1]==NULL )
    {
        numOfSets = 1;
    }

     
    setsChanged = 1;
    while( setsChanged == 1 )
    {
        setsChanged = 0;
        for( i=0; i<numOfSets; i++ )
        {
            DFASet_header   =   S[i];
            /* 只有集合一个元素，无须参与分裂 */
            if( DFASet_header == NULL || DFASet_header->next == NULL )
                continue;

            for( j=0; j<numOfConds; j++ )
            {
                cond    =   CondSet[j];
                /* S[i]集合中第一个状态经cond转化的状态ID记为temp */
                temp    =   Status_Transfer_Under_Condition( 
                                        dfa->Status[ DFASet_header->StatusID ],cond);

                newheader = NULL;
                for( p=DFASet_header->next; p!=NULL; p=p->next )
                {
                    if( Status_Transfer_Under_Condition(dfa->Status[p->StatusID],cond)!=temp)
                    {
                        setsChanged = 1;
                        if( p->next != NULL )
                        {
                            q = p;
                            p->next->front = p->front;
                            p->front->next = p->next;
                            q->next = NULL;
                            connect( &newheader, &q );
                            p = p->front;   //由于当前的p节点被删除,要回到上一个节点
                        }else{
                            q = p;
                            p->front->next = NULL;
                            connect( &newheader, &q );
                            p = p->front;
                        }
                        newheader->front = NULL;
                    }
                }
                if( newheader != NULL )
                {
                    S[ numOfSets++ ] = newheader;
                    break;
                }
            }

            if( setsChanged )
                break;

        }
    }

    for( i=0; i<numOfSets; i++ )
      for( p=S[i]; p!=NULL; p=p->next )
        Status_Set[p->StatusID] = i;
    

    slim_dfa    =   malloc( sizeof(DFA) );
    slim_dfa->numOfStatus = numOfSets;      /* 等价状态的集合数<=>最小化DFA状态数 */
    slim_dfa->Status    = malloc( sizeof(Status*) * slim_dfa->numOfStatus );
    isNullPointer( slim_dfa->Status );
    /* 初始化每个Status */
    for( i=0; i<slim_dfa->numOfStatus; i++ )
    {
        slim_dfa -> Status[i] = allocStatus();
        slim_dfa -> Status[i] -> ID = i;
    }

    /* 填表，并且创建边 */
    for( i=0; i<numOfSets; i++ )
    {
        
        for( j=0; j<numOfConds; j++ )
        {
            cond = CondSet[j];
            /* S[i]中的元素在cond转换下得到的状态 */
            /* 因为S[i]中的元素等价,所以只取第一个 */
            from_id = S[i]->StatusID;
            /* 得到转换后的状态 */
            to_id   = Status_Transfer_Under_Condition( dfa->Status[from_id], cond );
            /* 得到目标集合的ID */
            if( to_id!=-1 )
            {
                to_Set_ID = Status_Set[ to_id ];
                newEdge = CopyEdge( cond );
                edgeBuffer[ numOfEdgeBuffer++ ] = newEdge;
                linkTwoStatus_by_AnEdge( 
                        slim_dfa->Status[i],slim_dfa->Status[to_Set_ID],newEdge);
            }else{
            }
        }
    }

    slim_dfa->numOfEdges    = numOfEdgeBuffer;
    slim_dfa->edgeArray     = malloc( sizeof(Edge) * numOfEdgeBuffer );
    isNullPointer( slim_dfa->edgeArray );
    for( i=0; i<numOfEdgeBuffer; i++ )
        slim_dfa->edgeArray[i] = edgeBuffer[i];

    /* 寻找start 和 end */
    for( i=0; i<numOfSets; i++ )
    {
        for( p=S[i]; p!=NULL; p=p->next )
        {
            /* S[i] 与 slim_dfa->Status[i] 对应 */
            if( p->StatusID == dfa->start->ID )
                slim_dfa->start = slim_dfa->Status[i];
            if( FinalStatus[ p->StatusID ] == true )
                slim_dfa->Status[i]->FinalStatus = true;
        }
    }

    /* 重新分配状态 */
    reassignID( slim_dfa );
    reassign_edge_order( slim_dfa );
    return slim_dfa;
}
