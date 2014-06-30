#ifndef _LLEX_H_
#define _LLEX_H_

#include "typedef.h"
#include "DFAEntity.h"
#include <stdio.h>

/* lexfile必须要进行转义的字符： {  } 
 */

#define     HEADER_MAX  500
#define     LINE_MAXSIZE 100
#define     MAXLEN_RegName 80
#define     MAXLEN_Action   1500

#define     MAXSIZE_HASH    1500

/* 首先我们定义lexfile中诸如 Number  {digit}|{float}|0123 的整条记录为 实体
 * 一个实体中可以存在几个正则表达式，如Number实体就有三个正则表达式：
 *    digit, float, "0123"    ,其中{ name } 表示先前定义的正则表达式
 */

/* RegExpInfo 储存一个实体中的一条正则表达式，它可能是先前定义的，
 * 也可能是现有实体中具体的表达式（如0123）
 */

typedef struct RegexInfo{
    char        *name;              /* 具体正则式取名：  实体名字 + _concrete_ + 编号 */
    char        *concrete_rExp;     /*具体的正则表达式，如0123*/
    DFAEntity   *dfa_entity;  
    int         ReverseMatch;       /* 取反标志， 例如 {~digit}就是不匹配digit的任意串 */
}RegexInfo;


/* struct MatchEntity 代表每个实体
 * 例如实体：      FLOAT    (+|-)?{number}.{number}E
   
                 { FLOAT } { printf("FLOAT"); }

 * MatchEntity以下字段分别储存此条记录的如下信息：
 *      1. entityName是lexfile中定义的名字， 此处 entityName = "FLOAT"
 *      2. action 是 匹配后的执行代码
 *      3. RegexArray 记录 实体中所有正则式，这里包括 (+|-)? 、 number 、. 、number 还有 E
 */


typedef struct _match_entity{
    char        *entityName;
    char        *action;

    RegexInfo   RegexArray[100];
    int         numOfRegexArray;
}MatchEntity;


void    checkDFA_AND_text(
                        xchar_t *,
                        xchar_t *
                        );


/* 对于正则表达式MatchEntity中的DFA，过程generate_FinalStatusJudge
 * 为该DFA产生一个终点判别数组 FinalStatus[], 其中FinalStatus[i]
 * 为true表示 DFA的第i个状态是终点接受状态（i=0,1,2,...）
 */
void    generate_FinalStatusJudge( FILE *, MatchEntity );


/* 为MatchEntity中的DFA产生一个状态转换表T， 其中，
 * T[i][j] 表示DFA的第i个状态，在字符j（ASCII值）下能转换到的状态号 
 */
void    generate_T( FILE *, MatchEntity );



/* 为MatchEntity中产生一个识别函数，该识别函数的原型是
 * bool Recognize_for_%s( char *text ); 即对于任意串text，
 * 判别函数利用表T_%s（全局变量）执行一系列状态转换，若最终得到
 * 的状态是接受状态，Recognize_for_%s返回true 
 */
void    generate_Recognize( FILE *, MatchEntity );

/* 为MatchEntity产生识别和执行相应动作的函数 */
void    generate_Recognize_Act( FILE *, MatchEntity );

void    generate_HeaderInfo( FILE *, char [] ); 

/* 读取字符串，并且将所有正则表达式记录于MatchEntity类型的数组中,
 * 将所有头定义信息填入char数组中 
 */
void    ReadLexFile( char *filename, MatchEntity [], char HeaderDef[]);

void    read_Regular_Action( FILE *f, MatchEntity [] );


/* Array[]是散列槽，有许多空位置，而clear_null_slot的用途就是将
 * Array[]中所有的非空槽写进newArray, 而此后不再检索某个特定的MatchEntity
 */
void    
clear_null_slot( MatchEntity Array[],
                 MatchEntity newArray[],
                 int *
            );

/* 为MatchEntity中所有的正则表达式构造相应的DFA以及状态转换表T */
void construct_DFA(MatchEntity [], int);



/* 为MatchEntity数组中的每一个DFA分别构造： 终点状态判别表，状态转换表，识别函数 */
void generate_everything( FILE *, MatchEntity[], int, char[] );

/* 产生指向一个指针函数数组，每个指针分别指向一个Regexp对应的RecognizeAct_for_%s函数。
 * 以后当识别一个token时，将这个token分别与这个数组中每个函数进行识别
 */
void generate_function_array( FILE *f, int numOfArray );

/* 让函数指针数组中每一个元素ptr[i]分别指向 Array[]中每个元素的RecognizeAct_for_%s */
void generate_all_recognize_functions( FILE *f, MatchEntity Array[], int numOfArray );


#endif
