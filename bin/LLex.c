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

void    generate_FinalStatusJudge( FILE *f, RegexEntity regex )
{
    int i;
    fprintf(f,"static int FinalStatus_%s[]=\n{", regex.name);
    for( i=0; i<regex.dfa_entity->numOfStatus-1; i++ )
    {   
        if( regex.dfa_entity->dfa->Status[i]->FinalStatus == true )
            fprintf(f,"1, ");
        else
            fprintf(f,"0, ");
    }
    if( regex.dfa_entity->dfa->Status[i]->FinalStatus == true )
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

/*
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
*/


static int 
ReadBraceContent(FILE *f,char recv[])
{
    int c;
    int length, leftBracket;
    
    leftBracket = 1;
    length = 0;
    while( (c=fgetc(f))!= '}' && leftBracket > 0 )
    {
        if( c=='{' )
            leftBracket++;
        else if( c=='}' )
            leftBracket--;
        recv[length++] = c;
    }
    recv[length++] = '\0';
    return length;
    /* strip */
}

static void 
strip(char recv[])
{
    char buffer[100];
    int i,j,count;
    j = count = 0;
    for( i=0; i<strlen(recv); i++ )
    {
        if(count==0 && recv[i]==' ')
            continue;
        if(count==1 && recv[i]==' ')
            break;

        count = 1;
        buffer[j++] = recv[i];
    }
    buffer[j++] = '\0';
    strcpy(recv,buffer);
}

static void 
getString( FILE *f, char recv[] )
{
    int c;
    int i;
    while( (c=fgetc(f)) == ' ' ){}
    
    i = 0;
    recv[i++] = c;

    while( (c=fgetc(f)) != ' ' )
        recv[i++] = c;
    recv[i++] = '\0';

}


linkNode
ReadLexFile( char *filename, char HeaderDef[] )
{
    FILE    *f;
    char    c, last_c;
    char    ReadIn[100]; 
    xchar_t     ReadIn2[100];
    xchar_t     RegexBuff[ MAXLEN_REGEXP ],*A;

    char    buffer[3000];
    char    RegexNameBuff[MAXLEN_REGEXP_NAME];

    char    RegexActionBuff[3000];
    int     regexAction_length;

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
    int     i, length, regexName_length, regex_length;

    linkNode    root, findnode;
    
    RegexEntity currRegexEntity;

    f = fopen(filename,"r");

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
    while( 1 )
    {
        getString( f, RegexNameBuff );
        if( strcmp( RegexNameBuff, "%%" )==0 )
            break;

        currRegexEntity.name = malloc( sizeof(char) * strlen(RegexNameBuff) );
        isNullPointer( currRegexEntity.name );
        
        regex_length = 0; 
        while( (c=fgetc(f)) != ' ' )
        {
            if( c == '{' )
            {
                /* 寻找{ } 内指定的正则表达式名字 */
                regexName_length = ReadBraceContent( f, RegexNameBuff );
                findnode = FindEntNode( root, RegexNameBuff );

                if( findnode != NULL )
                {   
                    A = getRegexFromNode( findnode );
                    for( i=0; A[i]!='\0'; i++ )
                        RegexBuff[ regex_length++ ] = A[i];

                }else{
                    RegexBuff[ regex_length++ ] = '{';
                    for( i=0; RegexNameBuff[i]!='\0'; i++ )
                        RegexBuff[ regex_length++ ] = RegexNameBuff[ i ];
                    RegexBuff[ regex_length++ ] = '}';
                }

            }else{
                RegexBuff[ regex_length++ ] = c;        
            }
        }

        currRegexEntity.regex = malloc( sizeof(xchar_t) * wcslen(RegexBuff) );
        for( i=0; i<wcslen(RegexBuff); i++ )
            currRegexEntity.regex[i] = RegexBuff[i];

        if( root == NULL )
        {
            Insert_EntTree( root, currRegexEntity );
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
    while( 1 )
    {
        while( (c=fgetc(f)) != '{' );
        regexName_length = ReadBraceContent( f, RegexNameBuff );

        findnode = FindEntNode(root,RegexNameBuff);
        if( !findnode )
        {
            printf("不存在名为%s的正则表达式\n",RegexNameBuff);
            exit(1);
        }
        while( (c=fgetc(f)) != '{' );

        regexAction_length = ReadBraceContent( f, RegexActionBuff ); 

        findnode->regex_entity.action = malloc( sizeof(char)* regexAction_length );
        isNullPointer( findnode->regex_entity.action );

        /* 将ActionBuff储存的代码插入到相应的位置 */
        strcpy( findnode->regex_entity.action, RegexActionBuff ); 
    }
}

/*
void construct_DFA( RegexEntity Array[], int numOfArray )
{
    int i;
    for( i=0; i<numOfArray; i++ )
    {
        for( j=0; j<Array[i].numOfRegexArray; j++ )
        {
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


void generate_function_array( FILE *f, int numOfArray )
{
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
    fprintf(f,"int main(int argc, char *argv[])\n" 
    "{\n"
    "    char str[100];\n"
    "    FILE *inFile;\n"
    "    int    i;\n"
    );
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
*/
