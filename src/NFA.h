#ifndef _NFA_H
#define _NFA_H

#include "typedef.h"
#include <stdbool.h>

typedef struct NFA NFA;
typedef Status *VertexTable; 
typedef NFA     DFA;
typedef NFA     *pt_NFA;

// -----------------Edge 重构区 -------------------
#define E Edge
typedef struct E *E;

extern void outputEdgeCrossTable (E e);
extern E    allocEdge ();
extern E    addCharacter (E e, wchar_t c);
extern void addRange (E e, Range r);
extern int  crossEdge (E e, wchar_t c);
extern void setMatchRangeOrNot (E e, int);

/* 产生一条epsilon边 */
E    allocEpsilonEdge();

extern Edge allocEmptyEdge ();
extern bool EqualCondition (E, E); 
void printEdge (E);
E CopyEdge (E);
void freeEdge( E edge );

//-------------------Status 重构区--------------------

#define S   Status
typedef struct S *S;

/* 产生一个状态点 */
extern S    allocStatus ();

extern void outputStatus (S);
// 调试专用函数
extern Status   nextStatus (S, wchar_t);

extern void     printStatus (S s);

/* ------------------NFA 重构区 --------------------- */

typedef struct NFA  *NFA;

/* 根据匹配单个的字符创建只有一条边两个状态的NFA */
extern NFA  CreateOneNFA (wchar_t c);

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

/* 用一条边(edge)连结两个状态(status) */
void 
linkTwoStatus_by_AnEdge (Status from, Status to, Edge bridge);




extern NFA CreateOneNFA (wchar_t c);

/* 创建只有一条边，但无任何属性值的边。 */
extern NFA CreateNullNFA ();



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
generateCondSet (NFA, E, int* );



extern int  
Status_Transfer_Under_Condition(const Status status, E cond);


void freeNFA( NFA nfa );


#undef E
#endif
