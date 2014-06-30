#include "LLex.h"
#include "DFAEntity.h"
#include "NFA.h"
#include "nfa_to_dfa.h"
#include "typedef.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/* 检测DFA及文本的匹配情况 */
void checkDFA_AND_text( 
                        xchar_t *regularExpression,
                        xchar_t *normalText
                    )
{
    DFAEntity   entity;
    entity.dfa = Subset_Construct( RegexpToNFA(regularExpression) );
    
    entity.T = (DFATable) makeUpDFATable( entity.dfa );
    
    if(Recognition( entity, normalText ))
        wprintf(L"%ls match string %ls\n", regularExpression, normalText );
    else
        wprintf(L"%ls doesn't match string %ls\n", regularExpression, normalText );
}

void    generate_FinalStatusJudge( FILE *f, MatchEntity info )
{
    int i;
    fprintf(f,"static int FinalStatus_%s[]=\n{", info.name);
    for( i=0; i<info.entity->numOfStatus-1; i++ )
    {   
        if( info.entity->dfa->Status[i]->FinalStatus == true )
            fprintf(f,"1, ");
        else
            fprintf(f,"0, ");
    }
    if( info.entity->dfa->Status[i]->FinalStatus == true )
        fprintf(f,"1 }; \n");
    else
        fprintf(f,"0 }; \n");
}

/* 生成头定义信息 */
void    generate_HeaderInfo( FILE *f, char HeaderDef[] )
{
    fprintf( f,"%s",HeaderDef );
    fprintf( f,"%s\n","#include <stdbool.h>");
    fprintf( f,"%s\n","#include <string.h>");
    fprintf( f,"%s\n","#include <stdlib.h>");
}

/* 生成状态转换表 */
void    generate_T( FILE *f, MatchEntity ent )
{
    int i, j, regex_index;
    fprintf( f, "static int T_%s[][128]=\n",ent.name );
    fprintf( f,"{\n" );
    fprintf( f, "\t\t" );

    for( regex_index=0; regex_index<ent.numOfRegexArray; regex_index++ )
    {
        if( ent.RegexArray[regex_index].dfa_entity == NULL )
            continue;

        for( i=0; i<ent.RegexArray[regex_index].dfa_entity->numOfStatus; i++ )
        {
            fprintf(f,"{ ");
            for( j=0; j<128; j++ )
            {
                fprintf(f,"%d", ent.RegexArray[regex_index].dfa_entity->T[i][j]);
                if( j<127 )
                    fprintf(f,", ");
            }
            fprintf(f,"}");
            if( i < ent.RegexArray[regex_index].dfa_entity->numOfStatus - 1 )
                fprintf(f,",");
            fprintf(f,"\n");
        }
        fprintf(f,"};\n");
    }
}   

void    generate_concrete_Recognize( FILE *f, RegexInfo regex )
{
    fprintf(f,"bool Recognize_for_%s( char * text)\n"
    "{\n"
    "    int i,length;                \n"
    "    char c;                      \n"
    "    length = strlen( text );     \n"
    "\n"
    "    i = 0;                       \n"
    "    while( (c=(*text++))!='\\0' )\n"
    "    {                            \n"
    "        if( T_%s[i][c] == -1 )      \n"
    "            return false;        \n"
    "        else                     \n"
    "            i = T_%s[i][c];         \n"
    "    }                            \n"
    "    if( FinalStatus_%s[i]==true )             \n"
    "        return true;             \n"
    "    return false;                \n"
    "}                                \n"
    ,regex.name, regex.name, regex.name, regex.name
    );
}


void    generate_MatchEntity_Recognize( FILE *f, MatchEntity match_entity )
{
    int i;

    fprintf("int Recognize_for_%s(char *text)\n", match_entity.entityName);
    fprintf("{\n");
    fprintf("    if( ");
    for( i=0; i<match_entity.numOfRegexArray; i++ )
    {
        if( match_
        fprintf("Recognize_for_%s

    }

}

void    generate_Recognize_Act( FILE *f, MatchEntity info)
{
    char name[100];
    strcpy( name, info.name );
    if( !info.action )
    {
        fprintf(f,"int RecognizeAndAct_for_%s(char *text)\n"
        "{\n"
        "    return 0;\n"
        "}\n"
        , info.name
        );
    }
    else
    {
        fprintf(f,"int RecognizeAndAct_for_%s( char * text)\n"
        "{\n"
        "    if( Recognize_for_%s( text ) == true )\n"
        "    {\n"
        "       %s\n"
        "        return 1;\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
        , info.name, info.name, info.action
        );
    }
}


static int Hash_For_InfoArray_Index( char *str )
{
    int     i,Index;
    if( str==NULL ){    
        printf("检测到哈希函数中字符串为空，停止进程");
        exit(1);
    }
    Index = 0;
    for( i=0; i<strlen(str); i++ )
        Index += str[i];
    Index %= 1000;

    return Index;
}


void ReadLexFile( char *filename, MatchEntity MatchEntArray[], char HeaderDef[] )
{
    FILE    *f;
    char    c, last_c;
    char    ReadIn[100]; 
    xchar_t ReadIn2[100];
    char    buffer[3000];
    char    RegExpNameBuffer[MAXLEN_REGEXP_NAME];
    char    RegExpContentBuffer[MAXLEN_REGEXP]; 
    int     RegExpContentBuffer_Size;
    int     isReverseRegExp;
    char    opBuffer[3];
    int     RegExpNameBuffer_Size;
    int     rExpIndex;
    int     _buff;
    int     size_headerDef;
    int     Index, currentIndex;
    int     leftBracket;
    int     i, length;

    MatchEntity currentEntity;

    int     numOfRegInfoArray;

    f = fopen(filename,"r");
    /* 记录已读取的正则表达式的个数 */
    numOfRegInfoArray = 0;

    /* 首先读取 %{ 以及 %}内的全局定义 */
    fscanf(f,"%s", ReadIn);
    if( strcmp(ReadIn,"%{") != 0 )
    {
        printf("包含头文件和全局变量定义的开头符号应为%%f.\n");
        exit(1);
    }

    size_headerDef = 0;
    HeaderDef[ size_headerDef++ ] = ' ';
    while( (c=fgetc(f))!='%' )
    {
        last_c = HeaderDef[ size_headerDef-1 ];
        if( c==' ' && (last_c==' ' || last_c=='\n' ))
            continue;
        HeaderDef[ size_headerDef++ ] = c;
        if( size_headerDef > HEADER_MAX )
        {
            printf("头部定义过大\n");
            exit(1);
        }
    }
    HeaderDef[ size_headerDef++ ] = '\0';

    /* 观察头部定义外部格式是否正确 */
    if( (c=fgetc(f)) != '}' )
    {
        printf("%%后无}结尾!\n");
        exit(1);
    }

    /* 记录每个正则表达式的命名以及式子 */
    while( fscanf(f,"%s",ReadIn) && strcmp(ReadIn, "%%")!=0 )
    {

        /* 为了检索方便，正则表达式在Array中的储存位置与其name字符串值有关 */
        /* 这里采用了哈希函数计算 索引值，对于碰撞问题采用 下一位平移的方法 */
        /* 声明哈希函数，接收字符串，计算出该串在 InfoArray中的索引 */
        int     Hash_For_InfoArray_Index( char * );
        currentIndex   =   Hash_For_InfoArray_Index( ReadIn );

        /* 正则表达式表格中正则表达式的个数 */
        numOfRegExp_In_MatchEntity = 0;

        while( RegexpInfoArray[currentIndex].name != NULL )
            /* 散列碰撞，寻找下一个位置 */
            currentIndex ++;

        if( RegexpInfoArray[currentIndex].name == NULL )
        {
            /* Insert */
            RegexpInfoArray[currentIndex].name = malloc(sizeof( ReadIn ));
            strcpy( RegexpInfoArray[currentIndex].name, ReadIn );
        }

        currentEntity = RegexpInfoArray[currentIndex];
        currentEntity.numOf_rExp = 0;
        /* 读入正则表达式的内容 */

        fscanf(f,"%s",ReadIn);
        
        for ( i=0; ReadIn[i]!='\0'; i++ )
        {
            if( ReadIn[i] == '{' )
            {
                i++;
                RegExpNameBuffer_Size = 0;    
                while(  ReadIn[i]!='}' )
                {
                    if( ReadIn[i]=='~' )
                        isReverseRegExp = 1;
                    else
                        isReverseRegExp = 0;

                    if( ReadIn[i]=='\0' )
                    {
                        printf("不合法的提取正则表达式命名方式：无}结尾\n");
                        exit(1);
                    }
                    RegExpNameBuffer[ RegExpNameBuffer++ ] = ReadIn[i];
                    i++;
                }
                if( ReadIn[i+1] == '\0' )
                {
                    strcpy(opBuffer,"0");
                }
                else if( ReadIn[i+1] == '|' )
                {
                    strcpy(opBuffer,"||");
                    i++;  // 避免加入正则表达式内容 
                }
                else
                {
                    strcpy(opBuffer,"&&");
                }

                RegExpNameBuffer[ RegExpNameBuffer++ ] = '\0';
                /* 寻找{ } 内指定的正则表达式名字，这个正则表达式
                 * 必须在 之前出现 
                 */
                Index = Hash_For_InfoArray_Index( RegExpNameBuffer );

                while( RegexpInfoArray[Index].name != NULL )
                {
                /* 避免散列值相同的名字 */
                    if( strcpy( RegexpInfoArray[Index].name, RegExpNameBuffer )!=0 )
                        Index++;
                    else
                        break;
                }   
            
                if( RegexpInfoArray[Index].name == NULL )
                {
                    printf("不存在名为%s的正则表达式\n",RegExpNameBuffer);
                    exit(1);
                }
                /* 为当前的正则表达式的 ExpTable中第 numOfRegExp_In_MatchEntity 个
                 * 正则表达式分配空间
                 */
                currentEntity.ExpTable[ rExpIndex ][ 0 ] = 
                                        malloc( sizeof(char)*strlen(RegExpNameBuffer) );
                
                /* 储存正则表达式名字 */
                strcpy( 
                        currentEntity.ExpTable[ rExpIndex ][ 0 ], 
                        RegExpNameBuffer
                    );
                
                /* 运算符的最大长度是2 */
                currentEntity.ExpTable[ rExpIndex ][ 1 ] =
                                        malloc(sizeof(char)*3);

                strcpy( currentEntity.ExpTable[ rExpIndex ][ 1 ],
                        opBuffer
                    );

                currentEntity.ExpTable[ rExpIndex ][ 2 ] =
                                        malloc(sizeof(char)*2);
                if( isReverseRegExp == 1 )
                    strcpy( currentEntity.ExpTable[ rExpIndex ][ 1 ], "Y" );
                else
                    strcpy( currentEntity.ExpTable[numOfRegExp_In_MatchEntity][1], "N" );
            }

            else
            {
                RegExpContentBuffer_Size = 0;
                while( ReadIn[i]!='\0' && ReadIn[i]!='{' )
                {
                    RegExpContentBuffer[ RegExpContentBuffer_Size++ ] = ReadIn[i];
                }
                if ( ReadIn[i-1]=='|' )
                    RegExpContentBuffer[ --RegExpContentBuffer_Size ] = '\0';

                currentEntity.rExpContentArray[ currentEntity.numOf_rExp] 
                                        =  malloc( RegExpContentBuffer_Size );
                /* 储存 带有内容的正则表达式 于数组中 */
                strcpy( 
                        currentEntity.rExpContentArray[ currentEntity.numOf_rExp ], 
                        RegExpContentBuffer 
                );

                currentEntity.ExpTable[ rExpIndex ][ 0 ] =  
                                            malloc( sizeof(char) * strlen(currentEntity.name));
                strcpy( currentEntity.ExpTable[ rExpIndex ][0], currentEntity.name ); 
                    
                currentEntity.ExpTable[ rExpIndex ][ 1 ] = malloc( 2 );
                /* 第二项储存的是索引 */
                currentEntity.ExpTable[ rExpIndex ][ 1 ][ 0 ] = currentEntity.numOf_rExp;
                currentEntity.ExpTable[ rExpIndex ][ 1 ][ 1 ] = '\0';

                /* 第三项储存的是取正标志 */
                currentEntity.ExpTable[ rExpIndex ][ 2 ] = malloc( 2 );
                currentEntity.ExpTable[ rExpIndex ][ 2 ][ 0 ] = 'N';
                currentEntity.ExpTable[ rExpIndex ][ 2 ][ 1 ] = '\0';

            }
        }

    }
    
    /* 对于%%的检测，先搁置 */

    /* 记录所有正则表达式对应名字的匹配执行代码 */
    while( fscanf(f,"%s",ReadIn) && strcmp(ReadIn, "%%")!=0 )
    {
        length = strlen(ReadIn);
        if( ReadIn[0]=='{' && ReadIn[length-1] == '}' )
        {
            ReadIn[length-1] = '\0';
            for( i=0; i<length-1; i++ )
                ReadIn[i] = ReadIn[i+1];  
        }
        else{
            printf("格式错误（无}结尾）\n");
            exit(1);
        }
        Index = Hash_For_InfoArray_Index( ReadIn );

        while( strcmp( ReadIn, RegexpInfoArray[Index].name )!=0 )
            Index++;

        while( c!='{' )
            c = fgetc(f);
        
        _buff = 0;
        /* 左花括号存在则为1, 每遇到一个{, leftBracket++，遇到},leftBracket-- */
        leftBracket = 1;
        while( c=fgetc(f) )
        {
            if( c=='{' )
                leftBracket++;
            else if( c=='}' )
                leftBracket--;

            if( leftBracket == 0 )
                break;
            buffer[ _buff++ ] = c;
        }
        buffer[ _buff++ ] = '\0';

        RegexpInfoArray[ Index ].action = malloc( sizeof(char)*_buff );
        isNullPointer( RegexpInfoArray[ Index ].action );

        /* 将ActionBuff储存的代码插入到相应的位置 */
        strcpy( RegexpInfoArray[ Index ].action, buffer ); 

    }
}


void 
clear_null_slot( 
                MatchEntity Array[], 
                MatchEntity newArray[], 
                int *numOfArray 
            )
{
    int     i;
    *numOfArray = 0;
    for( i=0; i<MAXSIZE_HASH; i++ )
    {
        if( Array[i].name != NULL )
        {
            newArray[ (*numOfArray)++ ] = Array[i];
            Array[i].name = NULL;
            Array[i].RegularExpression=NULL;
            Array[i].action = NULL;
            Array[i].entity = NULL;
        }
    }
}


void construct_DFA( MatchEntity Array[], int numOfArray )
{
    /* RegexpInfoArray是散列槽，故遍历整个可索引范围 */
    int i;
    for( i=0; i<numOfArray; i++ )
    {
        for( j=0; j<Array[i].numOfRegexArray; j++ )
        {
            /* 对于匹配实体中每一个具体正则表达式构造DFA，以及转换表 */
            Array[i].RegexArray[j].dfa_entity =
                                        malloc(sizeof(struct DFAEntity));

            isNullPointer( Array[i].RegexArray[j].dfa_entity );

            Array[i].RegexArray[i].dfa_entity -> dfa =
                                        Subset_Construct(
                                            RegexpToNFA( Array[i].RegularExpression )
                                        );

            Array[i].RegexArray[i].dfa_entity -> T = 
                                        makeUpDFATable( Array[i].entity->dfa );

            Array[i].entity->numOfStatus = Array[i].entity->dfa->numOfStatus;
        }
    }
}


/* 定义一个指向bool (char* )的函数指针数组 */
void generate_function_array( FILE *f, int numOfArray )
{
    /* numOfArray是正则表达式的实体数目 */
    fprintf(f,"int (*ptr[%d])( char* );\n",numOfArray);
}

void generate_all_recognize_functions( FILE *f, MatchEntity Array[], int numOfArray )
{
    int i;
    fprintf(f,"void set_functions_for_allfunctions( )\n");
    fprintf(f,"{\n");
    for( i=0; i<numOfArray; i++ )
    {
        fprintf(f,"    ptr[%d] = RecognizeAndAct_for_%s;\n", i, Array[i].name );
    }
    fprintf(f,"}\n");
}

static void generate_T_test( FILE *f, char *name )
{
    fprintf(f,
    "\tprintf(\"构建正则表达式%s的转换表测试。\\n\");\n"
    "\twhile( scanf(\"%%d, %%c\", &i, &c)!=EOF )\n"
    "\t{\n"
    "\t\tprintf(\"%%d\\n\",T_%s[i][c]);\n"
    "\t}\n", name, name
    );

}

static void generate_Recognize_detection( FILE *f, MatchEntity R )
{
    if( !R.name || !R.RegularExpression )
        printf("正则表达式生成错误，无法生成识别函数。\n");

    fprintf(f,
    "    printf(\"检测Recognize_for_%s函数\\n>>  \");\n"
    "    while( scanf(\"%%s\",str) != EOF )\n"
    "    {\n"
    "        if( Recognize_for_%s(str) )\n"
    "        {\n"
    "            printf(\"%%s 匹配正则表达式 %%s\\n\", str, \"%ls\" );\n"
    "        }\n"
    "        else\n"
    "        {\n"
    "            printf(\"%%s 不匹配正则表达式 %%s\\n\", str, \"%ls\" );\n"
    "        }\n"
    "        printf(\">>  \");\n"
    "    }\n",
    R.name, R.name, R.RegularExpression, R.RegularExpression 
    ); 
}

static void generate_RecognizeAct_detection( FILE *f, int numOfArray )
{
    fprintf(f,
    "\t    printf(\">>>  \");\n"
    "\t    while( scanf(\"%%s\", str) != EOF )\n"
    "\t    {\n"
    "\t        for( i=0; i<%d; i++ )\n"
    "\t        {\n"
    "\t            if( ptr[i](str) )\n"
    "\t            {\n"
    "\t                printf(\"\\n\");\n"
    "\t                break;\n"
    "\t            }\n"
    "\t        }\n"
    "\t        printf(\">>>  \");\n"
    "\t    }\n",
    numOfArray
    );
}

void generate_everything( FILE *f, MatchEntity Array[], int numOfArray, char HeaderDef[] )
{
    int     i;
    generate_HeaderInfo( f, HeaderDef );
    fprintf(f,"\n");
    for( i=0; i<numOfArray; i++ )
    {
        generate_FinalStatusJudge( f, Array[i] );
        fprintf(f,"\n");
        generate_T( f, Array[i] );
        fprintf(f,"\n");
        generate_Recognize( f, Array[i] );
        fprintf(f,"\n");
        generate_Recognize_Act( f, Array[i] );
        fprintf(f,"\n");
    }
    generate_function_array( f, numOfArray );
    fprintf( f, "\n" );

    generate_all_recognize_functions( f, Array, numOfArray );
    fprintf( f, "\n" );
    /* 产生main函数 */
    fprintf(f,"int main(int argc, char *argv[])\n" 
    "{\n"
    "    char str[100];\n"
    "    FILE *inFile;\n"
    "    int    i;\n"
    );
    /* “调用” set_functions_for_allfunctions函数 */
    fprintf(f,"    set_functions_for_allfunctions();\n");
    fprintf(f,"    if( argc==1 )\n"
    "    {\n");
    generate_RecognizeAct_detection( f, numOfArray );
    fprintf(f,"    }\n");
    fprintf(f,"    else\n"
    "    {\n"
    "        inFile = fopen(argv[1],\"r\");\n"
    "        if( !inFile )\n"
    "        {\n"
    "            printf(\"无法打开待识别文件。\\n\");\n"
    "            exit(1);\n"
    "        }\n"
    "        while( fscanf(inFile,\"%%s\",str) != EOF )\n"
    "        {\n"
    "            for( i=0; i<%d; i++ )\n"
    "            {\n"
    "                if( ptr[i](str) )\n"
    "                {\n"
    "                    printf(\" \");\n"
    "                    break;\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "        printf(\"\\n\");\n"
    "    }\n", numOfArray );
    fprintf(f,"}\n");
}
