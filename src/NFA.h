#ifndef _NFA_H
#define _NFA_H

#include "Array.h"
#include "typedef.h"
#include <stdbool.h>
#include "Edge.h"
#include "Status.h"


typedef struct Automaton *NFA;

extern NFA  CreateNFA(int n, int e);
/* 根据匹配单个的字符创建只有一条边两个状态的NFA */
extern NFA  CreateSingleNFA (wchar_t c);

extern Status   getStartStatus (NFA);
extern Status   getEndStatus (NFA);
extern Edge     getEdge (NFA nfa, int i);

extern void linkTwoStatusInNFA (NFA nfa, int from, int to, int e);

/* 创建无边的NFA。 */
extern NFA  CreateNFA_without_edge ();

/* 连结前后两个NFA */
extern NFA  Link (NFA, NFA);

/* 并置前后两个NFA */
extern NFA  Union (NFA, NFA);

/* 闭包(重复0零或1次以上) */
extern NFA  Closure (NFA);

/* 重复1次或以上 */
extern NFA  Repeat_atleast_one (NFA);

/* 可选的正则表达式 */
extern NFA  Option (NFA);

extern void printNFA (NFA);


/* 用一条边(edge)连结两个状态(status) */
//void 
//linkTwoStatus_by_AnEdge (int, int, int);

bool isMatchInEdge( wchar_t c,
                    Edge   e
                    );


NFA  CopyNFA (NFA);

/* 第三个参数: 数组大小; */
void RangeCopy (Range* ,Range *,int );


void isNullPointer (void *);


/* epsilon closure */

bool
itemInSet( int id, int Set[], int size );

void 
generateCondSet (NFA, Edge*, int* );


extern int  
Status_Transfer_Under_Condition(const Status status, Edge cond);

extern void freeNFA (NFA);

#undef S
#endif
