#ifndef _NFA_H
#define _NFA_H

#include "Array.h"
#include "typedef.h"
#include <stdbool.h>

// -----------------Edge 重构区 -------------------
#define E Edge
typedef struct E *E;
#define S   Status
typedef struct S *S;
typedef E   Condition;

extern E    allocEdge ();
extern E    allocEpsilonEdge();

extern void addCharacter (E e, wchar_t c);
extern void addRange (E e, Range r);
extern int  crossEdge (E e, wchar_t c);
extern void setMatchRangeOrNot (E e, int);

extern void setFromToStatus (E e, Status from, Status to);

extern bool EqualCondition (E, E); 

extern void printEdge (E);
extern E    CopyEdge (E);
extern void freeEdge (E);

extern int sizeOfEdge();
extern S getfromStatus (E e);
extern S gettoStatus (E e);

extern void outputEdgeCrossTable (E e);
//-------------------Status 重构区--------------------

/* 产生一个状态点 */
extern S    allocStatus ();

extern void outputStatus (S);
// 调试专用函数
extern S    nextStatus (S, wchar_t);

extern void setStatusID (S s, int id);

extern Array_T getInEdges(S s);
extern Array_T getOutEdges(S s);

extern int  getStatusID(S s);
extern void ensureFinalStatus (S s);
extern void cancelFinalStatus (S s);
extern void appendInEdge (S s, E e);
extern void appendOutEdge (S s, E e);

extern bool  isFinalStatus (S s);
extern int sizeOfStatus();
/* ------------------NFA 重构区 --------------------- */

typedef struct NFA  *NFA;

/* 根据匹配单个的字符创建只有一条边两个状态的NFA */
extern NFA  CreateSingleNFA (wchar_t c);

/* 创建无边的NFA。 */
extern NFA CreateNFA_without_edge ();

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
void 
linkTwoStatus_by_AnEdge (Status from, Status to, Edge bridge);





bool isMatchInEdge( wchar_t c,
                    E   e
                    );

void printNFA   (NFA );

NFA  CopyNFA (NFA);


/* 第三个参数: 数组大小; */
void RangeCopy (Range* ,Range *,int );


void isNullPointer (void *);


/* epsilon closure */

bool
itemInSet( int id, int Set[], int size );

void 
generateCondSet (NFA, E*, int* );



extern int  
Status_Transfer_Under_Condition(const Status status, E cond);


extern void freeNFA (NFA nfa);


#undef E
#endif
