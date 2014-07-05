#ifndef _LLEX_H_
#define _LLEX_H_

#include "typedef.h"
#include "NFA.h"
#include "DFAEntity.h"
#include "nfa_to_dfa.h"
#include "global.h"
#include <stdio.h>


#define     HEADER_MAX  500
#define     LINE_MAXSIZE 100
#define     MAXLEN_RegName 80
#define     MAXLEN_Action   1500

#define     MAXSIZE_HASH    1500

/* struct RegexEntity 代表每个实体
 * 例如实体：      FLOAT    (+|-)?{number}.{number}E
   
                 { FLOAT } { printf("FLOAT"); }

 * RegexEntity以下字段分别储存此条记录的如下信息：
 *      1. name是lexfile中定义的名字， 此处 name = "FLOAT"
 *      2. action 是 匹配后的执行代码
 *      3. Regex 记录实体的正则式，Regex中可能包含以命名的正则表达式
 *               Regex正则表达式中已命名的正则表达式最后会展开：
 *             如 FLOAT     (+|-)?[0-9]+.[0-9]+E
 *          
 *          如果{}内的名字不存在，则保留原来的字串:
 *             如 Comment     {%.*%}
 */


typedef struct _regex_entity{
    char        *name;
    char        *action;
    xchar_t     *regex;
    DFAEntity   *dfa_entity;
}RegexEntity;
/* 利用二叉树储存所有实体，实体的名字（name）作为关键码
 * 小于关键码的作为左子结点，反之作为右子结点
 */

typedef struct _node {
    char            *key;
    RegexEntity     regex_entity;
    struct  _node   *lChild;
    struct  _node   *rChild;
}Node, *linkNode;

/* 初始化“实体树”，返回根结点 */
linkNode
Init_EntityTree( RegexEntity ent );

/* 往树中插入实体, 若已存在该实体（名字相同），return false */
bool
Insert_EntTree( linkNode root, RegexEntity ent );

/* 寻找给定名字的实体 */
linkNode
FindEntNode( linkNode root, char *name );

xchar_t*
getRegexFromNode( linkNode );


void    checkDFA_AND_text(
                        xchar_t *,
                        xchar_t *
                        );


/* 对于正则表达式RegexEntity中的DFA，过程generate_FinalStatusJudge
 * 为该DFA产生一个终点判别数组 FinalStatus[], 其中FinalStatus[i]
 * 为true表示 DFA的第i个状态是终点接受状态（i=0,1,2,...）
 */
void    generate_FinalStatusJudge( FILE *, RegexEntity );


/* 为RegexEntity中的DFA产生一个状态转换表T， 其中，
 * T[i][j] 表示DFA的第i个状态，在字符j（ASCII值）下能转换到的状态号 
 */
void    generate_T( FILE *, RegexEntity );


/* 为每个具体正则表达式产生一个识别函数，该识别函数的原型是
 * bool Recognize_for_%s( char *text ); 即对于任意串text，
 * 判别函数利用表T_%s（全局变量）执行一系列状态转换，若最终得到
 * 的状态是接受状态，Recognize_for_%s返回true 
 */
void    generate_Recognize( FILE *f, RegexEntity regex_entity ); 



/* 为RegexEntity产生识别和执行相应动作的函数 */
void    generate_Recognize_Act( FILE *, RegexEntity );

void    generate_HeaderInfo( FILE *, char [] ); 

/* 读取字符串，并且将所有正则表达式记录于linkNode类型的根结点上
 * 将所有头定义信息填入char数组中
 */
linkNode    
ReadLexFile( char *filename, char HeaderDef[]);

void    read_Regular_Action( FILE *f, RegexEntity [] );

/* 为RegexEntity中所有的正则表达式构造相应的DFA以及状态转换表T */
void construct_DFA(RegexEntity [], int);


/* 为RegexEntity数组中的每一个DFA分别构造： 终点状态判别表，状态转换表，识别函数 */
void generate_everything( FILE *, RegexEntity[], int, char[] );

/* 产生指向一个指针函数数组，每个指针分别指向一个Regexp对应的RecognizeAct_for_%s函数。
 * 以后当识别一个token时，将这个token分别与这个数组中每个函数进行识别
 */
void generate_function_array( FILE *f, int numOfArray );

/* 让函数指针数组中每一个元素ptr[i]分别指向 Array[]中每个元素的RecognizeAct_for_%s */
void generate_all_recognize_functions( FILE *f, RegexEntity Array[], int numOfArray );


#endif
