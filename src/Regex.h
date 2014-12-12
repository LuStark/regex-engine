
#ifndef _DFAENTITY_H_
#define _DFAENTITY_H_

#include "constant.h"
#include "NFA.h"
#include "DFA.h"

typedef int     **DFATable;
typedef int     **CapTable;

typedef struct Regex    Regex, *regexNode;

typedef enum NodeType{SIMPLE, OP, BOUND} NodeType;

struct Regex {
    char        *regex_str;
    DFATable    T;
    /* isCap is a 2d table where isCap[i][j] used to recognize whether 
     * character j should be captured when it reach state i.
     */
    CapTable    isCap;
    NFA         nfa;
    DFA         dfa;
};


extern regexNode
alloc_regexNode();

extern void
init_Regex(regexNode re, DFA dfa);

extern void
construct_table(regexNode re);

/* DFA自动机可以生成一个二维表, 也称为状态转换表 */
extern DFATable
makeUpDFATable (DFA dfa);

/* 识别函数可以针对一个串进行识别，返回true/false */
extern bool
Recognition (Regex  re, wchar_t *normal);

extern regexNode 
re_compile(wchar_t *pattern);

/* 对于给定文本T和正则表达式re, 求出匹配re的子串, 以首尾位置返回 
 * 若成功匹配，返回true
 */
extern bool 
re_match2(regexNode re, wchar_t *T, int *start, int *end);

extern bool
re_match (Regex re, wchar_t *str);

extern void
matchContext(wchar_t *T, regexNode re);

#endif
