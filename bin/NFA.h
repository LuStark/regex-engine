#ifndef _NFA_H
#define _NFA_H

#include "typedef.h"
#include <stdbool.h>

typedef struct Status Status;
typedef struct Edge Edge;
typedef struct NFA NFA;
typedef Status *VertexTable; 
typedef Edge    *Condition;
typedef NFA     DFA;

struct Status {
    Edge **InEdges;
    int numOfInEdges;
    Edge **OutEdges;
    int numOfOutEdges;
    bool FinalStatus;

    int ID;
};

struct Edge {
    /* matchRange 和 inMatchRange的内容不一定互补。
     * 范围的指定是由正则表达式 如[A-Z] 和 [^A-Z] 确定.
     * inMatchContent 的内容 可由matchContent复制。如：
      identifier = abc;    
      digit = ~identifier. 则digit中inMatchContent可由identifier复制.
    */

    Status *from_Status;
    Status *to_Status ;

    Range *matchRange ;
    int numOfMatchRange ;

    /* 凡是匹配matchContent字符串中的一个字符
     * 的字符，都能通过此边进行 转换 */
    xchar_t *matchContent ;
    /* 凡是不匹配inMatchContent字符串中的一个字符
     * 能通过此边进行 转换 */
    xchar_t *inMatchContent ;

    Range *inMatchRange ;   
    int numOfInMatchRange ;
    bool AllowedAnyChar;  /* 支持任意字符, 以正则表达式中的 . 表示 */
    bool hasEpsilon ;    /* 改变是否支持 epesilon 转换.*/ 

};

struct NFA {
    Status *start, *end;    /* NFA只有一个end,而DFA有多个end,所以DFA不使用end字段 */
    Edge **edgeArray ;
    Status **Status ; /* 首尾分别指向start,end */
    int numOfStatus ;
    int numOfEdges ;
};

/* 连结前后两个NFA */
extern NFA *Link( NFA* , NFA* );

/* 并置前后两个NFA */
extern NFA *Union( NFA *, NFA * );

/* 闭包(重复0零或1次以上) */
extern NFA *Closure( NFA * );

/* 重复1次或以上 */
extern NFA *Repeat_atleast_one( NFA * );

/* 可选的正则表达式 */
extern NFA *Option( NFA * );

/* 用一条边(edge)连结两个状态(status) */

void 
linkTwoStatus_by_AnEdge( 
            Status * , 
            Status * ,
            Edge   *
            );

/* 产生一条epsilon边 */
Edge*
allocEpsilonEdge( );

/* 产生一个状态点 */
Status*
allocStatus( ) ;

extern xchar_t
*PreprocessExpression( xchar_t *RegExp );

NFA *RegexpToNFA( 
        xchar_t  *RegExp
        );

/* 不接受任何转义字符 */
NFA *CharRange( xchar_t *RegExp, int start_offset );

NFA *CreateOneNFA( xchar_t c );

/* 创建只有一条边，但无任何属性值的边。 */
NFA *CreateNullNFA( );

// 调试专用函数
Status *nextStatus( Status *currStatus,
                   xchar_t recvChar
                );

void printEdge( Edge * );

bool isMatchInEdge( xchar_t c,
                    Edge   *e
                    );

void printStatus( Status *s );
void printNFA( NFA * );

void EdgeArrayAppend(Edge ***,int*, Edge *);

NFA *CopyNFA( NFA * );

Edge *CopyEdge( Edge * );


/* 第三个参数: 数组大小; */
void RangeCopy( Range* ,Range *,int );

void outputStatus( Status * );

void isNullPointer( void * );


/* epsilon closure */

bool
itemInSet( int id, int Set[], int size );

void 
generateCondSet(NFA *,Condition*, int* );

bool
isMatchCondition( Condition, xchar_t );


extern bool
EqualCondition( Condition , Condition); 

extern int  
Status_Transfer_Under_Condition(const Status *status, Condition cond);


#endif
