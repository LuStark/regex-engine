
#ifndef _DFAENTITY_H_
#define _DFAENTITY_H_

#include "constant.h"
#include "NFA.h"
#include "DFA.h"

typedef int     **DFATable;
typedef struct Regex    Regex, *regexNode;


struct Regex {
    DFATable    T;
    NFA         nfa_buffer;
    DFA         dfa;
    char        name;
    regexNode   left;
    regexNode   right;
    bool        yucha;
    char        regex_op;
};


extern regexNode
alloc_regexNode();

extern void
init_Regex(regexNode re, DFA dfa);

/* DFA自动机可以生成一个二维表, 也称为状态转换表 */
extern DFATable
makeUpDFATable (DFA dfa);

/* 识别函数可以针对一个串进行识别，返回true/false */
extern bool
Recognition (Regex  re, wchar_t *normal);

extern regexNode 
re_compile(wchar_t *pattern);

extern bool
re_match (Regex re, wchar_t *str);

#endif
