
#ifndef _DFAENTITY_H_
#define _DFAENTITY_H_

#include "constant.h"
#include "NFA.h"
#include "DFA.h"

typedef int     **DFATable;

typedef struct Regex {
    DFATable    T;
    DFA         dfa;
    char        name;
//    int         numOfStatus;
} Regex;


extern Regex   init_Regex(DFA dfa);

/* DFA自动机可以生成一个二维表, 也称为状态转换表 */
extern DFATable
makeUpDFATable (DFA dfa);

/* 识别函数可以针对一个串进行识别，返回true/false */
extern bool
Recognition (Regex  re, wchar_t *normal);

extern Regex 
re_compile(wchar_t *pattern);

extern bool
re_match (Regex re, wchar_t *str);

#endif
