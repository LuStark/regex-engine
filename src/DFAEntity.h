/* 
 * DFAEntity是一个包含有自动机，表格，名字，状态数的结构体
 */


#ifndef _DFAENTITY_H_
#define _DFAENTITY_H_

#include "constant.h"
#include "NFA.h"

typedef int     **DFATable;

typedef struct dfa_entity {
    DFATable    T;
    DFA         *dfa;
    char        *name;
    int         numOfStatus;
} DFAEntity;

/* DFA自动机可以生成一个二维表, 也称为状态转换表 */
DFATable
makeUpDFATable( DFA *dfa );

/* 识别函数可以针对一个串进行识别，返回true/false */
bool
Recognition( DFAEntity entity, xchar_t *normal );

#endif
