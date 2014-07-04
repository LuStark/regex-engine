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

void    generate_T( FILE *f, RegexEntity ent )
{
    int i, j;
    fprintf( f, "static int T_%s[][128]=\n", ent.name );
    fprintf( f,"{\n" );
    fprintf( f, "\t\t" );

    for( i=0; i<ent.dfa_entity->numOfStatus; i++ )
    {
        fprintf(f,"{ ");
        for( j=0; j<128; j++ )
        {
            fprintf(f,"%d", ent.dfa_entity->T[i][j]);
            if( j<127 )
                fprintf(f,", ");
        }
        fprintf(f,"}");
        if( i < ent.dfa_entity->numOfStatus - 1 )
            fprintf(f,",");
        fprintf(f,"\n");
    }
    fprintf(f,"};\n");
}   

void    generate_Recognize( FILE *f, RegexEntity regex_entity )
{
    fprintf(f,"int Recognize_for_%s( char * text)\n"
    "{\n"
    "    int match_length, has_matched, t;            \n"
    "    char *p, c;                      \n"
    "    match_length = 0;\n"
    "\n"
    "    t = 0;\n"
    "    p = text;\n"
    "    has_matched = 0;\n"
    "    while( (*p)!='\\0' )\n"
    "    {\n"
    "        t = T_%s[t][*p];\n"
    "        if( t == -1 )\n"
    "        {\n"
    "            if( has_matched )\n"
    "            {\n"
    "                return match_length;\n"
    "            }\n"
    "            else\n"
    "                return -1;\n"
    "        }\n"
    "        else\n"
    "        {\n"
    "            match_length++;\n"
    "            if( FinalStatus_%s[t]==true )\n"
    "                has_matched = 1;\n"
    "        }\n"
    "        p++;\n"
    "    }                            \n"
    "    if( FinalStatus_%s[t]==true )             \n"
    "        return match_length;             \n"
    "    return -1;                \n"
    "}                                \n"
    ,regex_entity.name, 
    regex_entity.name, 
    regex_entity.name,
    regex_entity.name
    );
}

void    generate_Recognize_Act( FILE *f, RegexEntity regex_entity)
{
    if( !regex_entity.action )
    {
        fprintf(f,"int RecognizeAndAct_for_%s( )\n"
        "{\n"
        "    return 0;\n"
        "}\n"
        , regex_entity.name
        );
    }
    else
    {
        fprintf(f,"int RecognizeAndAct_for_%s( )\n"
        "{\n"
        "    if( true )\n"
        "    {\n"
        "        %s\n"
        "        return 1;\n"
        "    }\n"
        "    return 0;\n"
        "}\n"
        , regex_entity.name, regex_entity.action
        );
    }
}


static int 
ReadBraceContent(FILE *f,char recv[])
{
    int c;
    int length, leftBracket;
    
    leftBracket = 1;
    length = 0;
    while( (c=fgetc(f))!= '}' && c!='\n' && leftBracket > 0 )
    {
        if( c=='{' )
            leftBracket++;
        else if( c=='}' )
            leftBracket--;
        recv[length++] = c;
    }
    if( c=='\n' || c=='}' )
        ungetc( c, f );
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
        if(count==0 && (recv[i]==' '||recv[i]=='\n'))
            continue;
        if(count==1 && (recv[i]==' '||recv[i]=='\n'))
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
    /* 忽略前面的空格和换行 */
    while( (c=fgetc(f)) == ' ' || c=='\n' ){}
    
    i = 0;
    recv[i++] = c;

    while( (c=fgetc(f)) != ' ' && c!='\n' )
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
    if( f==NULL )
    {
        printf("不存在名为%s的文件。\n", filename );
        exit(1);
    }

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
        strcpy( currRegexEntity.name, RegexNameBuff );
        
        regex_length = 0; 
        /* 跳过名字与正则式之间的空格 */
        while( (c=fgetc(f)) == ' ' );

        while( c!=' ' && c!='\n' )
        {
            if( c == '{' )
            {
                /* 寻找{ } 内指定的正则表达式名字 */
                regexName_length = ReadBraceContent( f, RegexNameBuff );
                strip( RegexNameBuff );
                findnode = FindEntNode( root, RegexNameBuff );

                if( findnode != NULL )
                {   
                    A = getRegexFromNode( findnode );
                    for( i=0; A[i]!='\0'; i++ )
                        RegexBuff[ regex_length++ ] = A[i];
                    c = fgetc(f);
                    
                }else{
                    RegexBuff[ regex_length++ ] = '{';
                    for( i=0; RegexNameBuff[i]!='\0'; i++ )
                        RegexBuff[ regex_length++ ] = RegexNameBuff[ i ];
                }
            }
            else{
                RegexBuff[ regex_length++ ] = c;        
            }

            c = fgetc(f);
        }
        RegexBuff[ regex_length++ ] = '\0';
    
        currRegexEntity.regex = malloc( sizeof(xchar_t) * regex_length );
        wcscpy( currRegexEntity.regex, RegexBuff );
        //wprintf(L"%ls\n",currRegexEntity.regex);
        //while(1){}


        if( root == NULL )
        {
            root = Init_EntityTree( currRegexEntity );
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
    RegexNameBuff[0] = '\0';
    while( 1 )
    {
        while( (c=fgetc(f))==' ' || c=='\n' || c=='}' );
        if( c=='{' )
        {
            ReadBraceContent( f, RegexNameBuff );
            strip(RegexNameBuff);
        }else{
            if( c=='%' ){
                if( (c=fgetc(f))=='%' ){
                    break;
                }else{
                    printf("格式错误：无双%%\n");
                    exit(1);
                }
            }else{
                printf("格式错误：无%%结尾\n");
                exit(1);
            }
        }
        findnode = FindEntNode(root,RegexNameBuff);
        if( !findnode )
        {
            printf("%d\n",strlen(RegexNameBuff));
            printf("不存在名为%s的正则表达式\n",RegexNameBuff);
            exit(1);
        }

        while( (c=fgetc(f))==' ' || c=='\n' || c=='}' );
        if( c=='{' )
        {
            regexAction_length = ReadBraceContent( f, RegexActionBuff ); 

            findnode->regex_entity.action = malloc( sizeof(char)* regexAction_length );
            isNullPointer( findnode->regex_entity.action );
            /* 将ActionBuff储存的代码插入到相应的位置 */
            strcpy( findnode->regex_entity.action, RegexActionBuff ); 
        }
        else{
            if( c=='%' ){
                if( (c=fgetc(f))=='%' ){
                    break;
                }else{
                    printf("格式错误：无双%%\n");
                    exit(1);
                }
            }else{
                printf("格式错误：无%%结尾\n");
                exit(1);
            }
        }
    }
    return root;
}

void construct_DFA( RegexEntity Array[], int numOfArray )
{
    int i;
    for( i=0; i<numOfArray; i++ )
    {
        Array[i].dfa_entity = malloc(sizeof(DFAEntity));

        isNullPointer( Array[i].dfa_entity );

        Array[i].dfa_entity -> dfa = Subset_Construct(
                                            RegexpToNFA( Array[i].regex )
                                        );

        Array[i].dfa_entity -> T = makeUpDFATable( Array[i].dfa_entity->dfa );

        Array[i].dfa_entity->numOfStatus = Array[i].dfa_entity->dfa->numOfStatus;
        
    }
}

void generate_function_array( FILE *f, int numOfArray )
{
    fprintf(f,"int  (*ptr[%d])( char* );\n", numOfArray);
    fprintf(f,"int  (*ptr_action[%d])( );\n", numOfArray);
}

void generate_all_recognize_functions( FILE *f, RegexEntity Array[], int numOfArray )
{
    int i;
    fprintf(f,"void set_functions_for_allfunctions( )\n");
    fprintf(f,"{\n");
    for( i=0; i<numOfArray; i++ )
    {
        fprintf(f,"    ptr[%d] = Recognize_for_%s;\n", i, Array[i].name );
        fprintf(f,"    ptr_action[%d] = RecognizeAndAct_for_%s;\n", i, Array[i].name );
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

static void generate_Recognize_detection( FILE *f, RegexEntity ent )
{
    if( !ent.name || !ent.regex )
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
    ent.name, ent.name, ent.regex, ent.regex
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

void generate_scan( FILE *f )
{
    fprintf(f,
    "void scan(char *filename)\n"
    "{\n"
    "    char    Program[3000];\n"
    "    FILE    *f;\n"
    "    int     i = 0, c, nextc, scanPos, regexID, matchLen;\n"
    "\n"
    "    f = fopen(filename, \"r\");\n"
    "    if( !f ){\n"
    "        printf(\"文件无法打开。\\n\");"
    "    exit(1);\n"
    "    }\n"
    "    while( (c=fgetc(f)) != EOF )\n"
    "    {\n"
    "        if( c=='\\n' )\n"
    "        {\n"
    "            nextc = fgetc( f );\n"
    "            if( nextc != EOF )\n"
    "                ungetc( nextc, f);\n"
    "            else\n"
    "                break;\n"
    "        }\n"
    "        Program[i++] = c;\n"
    "    }\n"
    "    Program[i++] = '\\0';\n"
    "\n"
    "    scanPos = 0;\n"
    "    while( Program[scanPos]!='\\0' )\n"
    "    {\n"
    "        for( i = 0; i < numOfRegex; i++ )\n"
    "        {\n"
    "            matchlength_for_regex[i] = ptr[i]( Program + scanPos );\n"
    "        }\n"
    "        regexID = FindMaxIndex( matchlength_for_regex, numOfRegex );\n"
    "        if( (matchLen = matchlength_for_regex[regexID]) > 0 )\n"
    "        {\n"
    "            ptr_action[regexID]();\n"
    "        }else{\n"
    "            printf(\"无法向下分析.\\n\");\n"
    "            exit(1);\n"
    "        }\n"
    "        for( i=0; i<numOfRegex; i++ ) matchlength_for_regex[i]=0;\n"
    "        scanPos += matchLen;\n"
    "    }\n"
    "}\n");
}

void generate_global_variable( FILE *f, int numOfRegex )
{
    fprintf(f,
        "#define    numOfRegex  %d\n"
        "int    matchlength_for_regex[numOfRegex];\n",
        numOfRegex
        );

}

void generate_help_function( FILE *f )
{
    fprintf(f,
    "int FindMaxIndex( int A[], int n )\n"
    "{\n"
    "    int max_i;\n"
    "    int max;\n"
    "\n"
    "    int i;\n"
    "\n"
    "    max = -1;\n"
    "    max_i = 0;\n"
    "    for( i=0; i<n; i++ )\n"
    "    {\n"
    "        if( A[i]>max )\n"
    "        {\n"
    "            max = A[i];\n"
    "            max_i = i;\n"
    "        }\n"
    "    }\n"
    "    return max_i;\n"
    "}\n"
    );
}

void generate_everything( FILE *f, RegexEntity Array[], int numOfArray, char HeaderDef[] )
{
    int     i;
    generate_HeaderInfo( f, HeaderDef );
    fprintf(f,"\n");
    generate_global_variable( f, numOfArray );
    fprintf(f,"\n");
    generate_help_function( f );
    fprintf(f,"\n");
    generate_function_array( f, numOfArray );
    fprintf(f,"\n");

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
    fprintf( f, "\n" );
    generate_all_recognize_functions( f, Array, numOfArray );
    fprintf( f, "\n" );
    generate_scan( f );
    fprintf( f, "\n" );
    /* 产生main函数 */
    fprintf(f,"int main(int argc, char *argv[])\n" 
    "{\n"
    "    char fileName[101];\n"
    "    set_functions_for_allfunctions();\n"
    "    if( argc>1 )\n"
    "    {\n"
    "        scan( argv[1] );\n"
    "    }\n"
    "    else\n"
    "    {\n"
    "        scanf(\"%%s\", fileName);\n"
    "        if( strlen(fileName) >= 101 )\n"
    "        {\n"
    "            exit(1);\n"
    "        }\n"
    "        scan( fileName );\n"
    "    }\n"
    "    printf(\"\\n\");\n"
    "}\n"
    );
}

void StoreInList( linkNode root, RegexEntity Array[], int *numOfArray )
{
    linkNode    Stack[200], p;
    int         top;

    top = 0;

    p = root;

    while( p || top>0 )
    {
        if( p )
        {   
            Stack[top++] = p;
            p = p->lChild;
        }
        else
        {
            p = Stack[--top];
            Array[ (*numOfArray)++ ] = p->regex_entity;
            p = p->rChild;
        }
    }
}
