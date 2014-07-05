#ifndef _Status_Set_H
#define _Status_Set_H

#include "vector.h"
#include "status_set.h"
#include "NFA.h"

typedef struct {
    vector_int  eps_closure ;   //ε闭包
    vector_int  StatusID;      //状态ID集合
} StatusSet;

/* DFA 状态集合（链表形式） */
typedef struct node {
    int     StatusID;
    struct  node    *next; 
    struct  node    *front;
}*StatusSet_linkNode, *StatusSet_listHeader;

typedef StatusSet   SingleStatusSet,MultiStatusSet;


extern  StatusSet
getEpsilonClosure( NFA*, int );


/* 相等_状态集 */
extern  bool
isEqual_StatusSet(
                const   StatusSet , 
                const   StatusSet
                );

StatusSet   
StatusSet_Transfer( 
                NFA*,
                SingleStatusSet[],
                StatusSet,
                Condition
                );
/* 调试函数 */
extern void
printStatusSet( StatusSet s );


extern void 
LinkList_Insert( 
        StatusSet_listHeader *L, 
        int item 
        );

extern void
connect( 
        StatusSet_listHeader *,
        StatusSet_linkNode  *
        );
#endif
