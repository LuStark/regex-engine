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

void    generate_FinalStatusJudge( FILE *f, RegexEntity info )
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
void    generate_T( FILE *f, RegexEntity ent )
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

void    generate_Regex_Recognize( FILE *f, RegexEntity regex_entity )
{
    fprintf(f,"bool Recognize_for_%s( char * text)\n"
    "{\n"
    "    int i, length, t;            \n"
    "    char c;                      \n"
    "    length = strlen( text );     \n"
    "\n"
    "    i = 0;                       \n"
    "    while( (c=(*text++))!='\\0' )\n"
    "    {                            \n"
    "        t = T_%s[i][c];          \n"
    "        if( t == -1 )            \n"
    "            return false;        \n"
    "        else                     \n"
    "            i = t;                 \n"
    "    }                            \n"
    "    if( FinalStatus_%s[i]==true )             \n"
    "        return true;             \n"
    "    return false;                \n"
    "}                                \n"
    ,regex_entity.name, regex_entity.name, regex_entity.name
    );
}

void    generate_Recognize_Act( FILE *f, RegexEntity regex_entity)
{
    if( !regex_entity.action )
    {
        fprintf(f,"int RecognizeAndAct_for_%s(char *text)\n"
        "{\n"
        "    return 0;\n"
        "}\n"
        , regex_entity.name
        );
    }
    else
    {
        fprintf(f,"int RecognizeAndAct_for_%s( char * text)\n"
        "{\n"
        "    if( Recognize_for_%s( text ) == true )\n"
        "    {\n"
        "        %s\n"
        "        return 1;\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
        , regex_entity.name, regex_entity.name, regex_entity.action
        );
    }
}


static int
ReadBraceContent( const char regexString[], int start_offset, char regexName )
{
    

}


linkNode
ReadLexFile( char *filename, char HeaderDef[] )
{
    FILE    *f;
    char    c, last_c;
    char    ReadIn[100]; 
    xchar_t ReadIn2[100];
    xchar_t     regexBuffer[ MAXLEN_REGEXP ];

    char    buffer[3000];
    char    RegexNameBuff[MAXLEN_REGEXP_NAME];
    char    RegExpContentBuffer[MAXLEN_REGEXP]; 
    int     RegExpContentBuffer_Size;
    int     isReverseRegExp;
    char    op;
    int     RegexNameBuff_Size;
    int     rExpIndex;
    int     _buff;
    int     size_headerDef;
    int     Index, currentIndex;
    int     leftBracket;
    int     i, length, regexName_length;

    linkNode    root, findnode;
    
    RegexEntity currRegexEntity;

    f = fopen(filename,"r");
    /* 记录已读取的正则表达式的个数 */
    numOfRegInfoArray = numOfRegexEntityArray = 0;

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
    currRegexEntity.numOfRegexArray = 0;
    while( fscanf(f,"%s",ReadIn) && strcmp(ReadIn, "%%")!=0 )
    {
        currRegexEntity.name = malloc( sizeof(char) * strlen(ReadIn) );
        
        isNullPointer( currRegexEntity.name );
        
        fscanf(f,"%s",ReadIn);
        wcscpy( regexBuffer, ReadIn );

        for ( i=0; ReadIn[i]!='\0'; i++ )
        {
            if( ReadIn[i] == '{' )
            {
                /* 寻找{ } 内指定的正则表达式名字 */
                regexName_length = ReadBraceContent( ReadIn, i+1, RegexNameBuff );
                findnode = Find( root, RegexNameBuff );

                if( findnode != NULL )
                {   
                    /* 用正则表达式的内容 代替 {}内的命名 */
                    replaceString( regexBuffer, 
                                    i, 
                                    i + regexName_length, 
                                    getRegexFromNode( findnode )
                                );

                }
                i += regexName_length;
            }
        }
        if( root == NULL )
        {
            root = Insert_EntTree( currRegexEntity );
        }
        else if( Insert_EntTree(root, currRegexEntity) != true )
        {
            printf("无法插入实体信息。\n");
            exit(1);
        }
    }
    
    /* 对于%%的检测，先搁置 */

    /* 记录所有正则表达式对应名字的匹配执行代码 
     * 如   {digit}     { printf("DIGIT"); } 

     */
    while( fscanf(f,"%s",ReadIn) && strcmp(ReadIn, "%%")!=0 )
    {
       
        regexName_length = ReadBraceContent( ReadIn, 0, RegexNameBuff );

        findnode = Find(root,RegexNameBuff);
    
        if( !findnode )
        {
            printf("不存在名为%s的正则表达式\n",RegexNameBuff);
            exit(1);
        }


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

static void 
RecordRegexEntity( char *regexString, RegexEntity MatchEntArray[] )
{



}


void construct_DFA( RegexEntity Array[], int numOfArray )
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

void generate_all_recognize_functions( FILE *f, RegexEntity Array[], int numOfArray )
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


void generate_everything( FILE *f, RegexEntity Array[], int numOfArray, char HeaderDef[] )
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
