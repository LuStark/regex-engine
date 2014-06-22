#ifndef _LLEX_H_
#define _LLEX_H_

#include "typedef.h"
#include "DFAEntity.h"
#include <stdio.h>


#define     HEADER_MAX  500
#define     LINE_MAXSIZE 100
#define     MAXLEN_RegName 80
#define     MAXLEN_Action   1500

#define     MAXSIZE_HASH    1500

/* 识别-执行 实体 */
/* 读取的每个正则表达式都会记录在Recog_Exec数据结构中 */
typedef struct _regexp_info{
    char        *name;
    xchar_t     *RegularExpression;
    char        *action;
    DFAEntity   *entity;
}Regexp_Info;


void    checkDFA_AND_text(
                        xchar_t *,
                        xchar_t *
                        );


/* 对于正则表达式Regexp_Info中的DFA，过程generate_FinalStatusJudge
 * 为该DFA产生一个终点判别数组 FinalStatus[], 其中FinalStatus[i]
 * 为true表示 DFA的第i个状态是终点接受状态（i=0,1,2,...）
 */
void    generate_FinalStatusJudge( FILE *, Regexp_Info );


/* 为Regexp_Info中的DFA产生一个状态转换表T， 其中，
 * T[i][j] 表示DFA的第i个状态，在字符j（ASCII值）下能转换到的状态号 
 */
void    generate_T( FILE *, Regexp_Info );




/* 为Regexp_Info中产生一个识别函数，该识别函数的原型是
 * bool Recognize_for_%s( char *text ); 即对于任意串text，
 * 判别函数利用表T_%s（全局变量）执行一系列状态转换，若最终得到
 * 的状态是接受状态，Recognize_for_%s返回true 
 */
void    generate_Recognize( FILE *, Regexp_Info );

/* 为Regexp_Info产生识别和执行相应动作的函数 */
void    generate_Recognize_Act( FILE *, Regexp_Info );

void    generate_HeaderInfo( FILE *, char [] ); 

/* 读取字符串，并且将所有正则表达式记录于Regexp_Info类型的数组中,
 * 将所有头定义信息填入char数组中 
 */
void    ReadLexFile( char *, Regexp_Info [], char []);

/* Array[]是散列槽，有许多空位置，而clear_null_slot的用途就是将
 * Array[]中所有的非空槽写进newArray, 而此后不再检索某个特定的Regexp_Info
 */
void    
clear_null_slot( Regexp_Info Array[],
                 Regexp_Info newArray[],
                 int *
            );

/* 为Regexp_Info中所有的正则表达式构造相应的DFA以及状态转换表T */
void construct_DFA(Regexp_Info [], int);



/* 为Regexp_Info数组中的每一个DFA分别构造： 终点状态判别表，状态转换表，识别函数 */
void generate_everything( FILE *, Regexp_Info[], int, char[] );

/* 产生指向一个指针函数数组，每个指针分别指向一个Regexp对应的RecognizeAct_for_%s函数。
 * 以后当识别一个token时，将这个token分别与这个数组中每个函数进行识别
 */
void generate_function_array( FILE *f, int numOfArray );

/* 让函数指针数组中每一个元素ptr[i]分别指向 Array[]中每个元素的RecognizeAct_for_%s */
void generate_all_recognize_functions( FILE *f, Regexp_Info Array[], int numOfArray );


#endif
