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

void    generate_FinalStatusJudge( FILE *f, Regexp_Info info )
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
void    generate_T( FILE *f, Regexp_Info info )
{
    int i, j;
    fprintf( f, "static int T_%s[][128]=\n",info.name );
    fprintf( f,"{\n" );
    fprintf( f, "\t\t" );
    for( i=0; i < info.entity->numOfStatus; i++ )
    {
        fprintf(f,"{ ");
        for( j=0; j<128; j++ )
        {
            fprintf(f,"%d",info.entity->T[i][j]);
            if( j<127 )
                fprintf(f,", ");
        }
        fprintf(f,"}");
        if( i < info.entity->numOfStatus-1 )
            fprintf(f,",");
        fprintf(f,"\n");
    }
    fprintf(f,"};\n");
}

void    generate_Recognize( FILE *f, Regexp_Info info )
{
    char name[100];
    strcpy( name, info.name );
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
    ,name,name,name,name);
}

void    generate_Recognize_Act( FILE *f, Regexp_Info info)
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


void ReadLexFile( char *filename, Regexp_Info RegexpInfoArray[], char HeaderDef[] )
{
    FILE    *f;
    char    c, last_c;
    char    ReadIn[100]; 
    xchar_t ReadIn2[100];
    char    buffer[3000];
    int     _buff;
    int     size_headerDef;
    int     Index;
    int     leftBracket;
    int     i, length;

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
        Index   =   Hash_For_InfoArray_Index( ReadIn );

        while( RegexpInfoArray[ Index ].name != NULL )
            Index ++;

        if( RegexpInfoArray[ Index ].name == NULL )
        {
            /* Insert */
            RegexpInfoArray[ Index ].name = malloc(sizeof( ReadIn ));
            strcpy( RegexpInfoArray[ Index ].name, ReadIn );
        }

        /* 读入正则表达式的内容 */

        fscanf(f,"%s",ReadIn);
        
        /* RegularExpression采用双字节的宽字符 */
        RegexpInfoArray[Index].RegularExpression = malloc( 2*sizeof( ReadIn ));
        for ( i=0; ReadIn[i]!='\0'; i++ )
            RegexpInfoArray[Index].RegularExpression[i] = ReadIn[i]; 

        RegexpInfoArray[Index].RegularExpression[i] = '\0';
        //wcscpy( RegexpInfoArray[Index].RegularExpression, ReadIn2 );
    }
    
    /* 对于%%的检测，先搁置 */

    /* 记录所有正则表达式的匹配执行代码 */
    while( fscanf(f,"%s",ReadIn) && strcmp(ReadIn, "%%")!=0 )
    {
        length = strlen(ReadIn);
        if( ReadIn[0]=='{' && ReadIn[length-1] == '}' )
        {
            ReadIn[length-1] = '\0';
            for( i=0; i<length-1; i++ )
                ReadIn[i] = ReadIn[i+1];  
        }else{
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
                Regexp_Info Array[], 
                Regexp_Info newArray[], 
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


void construct_DFA( Regexp_Info Array[], int numOfArray )
{
    /* RegexpInfoArray是散列槽，故遍历整个可索引范围 */
    int i;
    for( i=0; i<numOfArray; i++ )
    {
        Array[i].entity = malloc( sizeof( DFAEntity ));
        isNullPointer( Array[i].entity );

        Array[i].entity->dfa = Subset_Construct(
                                            RegexpToNFA( Array[i].RegularExpression )
                                        );
        Array[i].entity->T = makeUpDFATable( Array[i].entity->dfa );
        Array[i].entity->numOfStatus = Array[i].entity->dfa->numOfStatus;
    }
}


/* 定义一个指向bool (char* )的函数指针数组 */
void generate_function_array( FILE *f, int numOfArray )
{
    /* numOfArray是正则表达式的实体数目 */
    fprintf(f,"int (*ptr[%d])( char* );\n",numOfArray);
}

void generate_all_recognize_functions( FILE *f, Regexp_Info Array[], int numOfArray )
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

static void generate_Recognize_detection( FILE *f, Regexp_Info R )
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

void generate_everything( FILE *f, Regexp_Info Array[], int numOfArray, char HeaderDef[] )
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
