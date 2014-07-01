#include "global.h"
#include "LLex.h"
#include "typedef.h"
#include "DFAEntity.h"
#include <stdio.h>
#include <string.h>

xchar_t *RegExp[ MAXNUM_REGEXP ];

Index    current_RegExp_Index ;
Index    current_RegExpArray_Index ;

int  numOfRegExps;

int main(int argc, char *argv[])
{
    int     numOfArray;
    FILE    *file_lexyyc;
    char    HeaderDef[ HEADER_MAX ];
    char    fileName[100];
    file_lexyyc = fopen("lex.yy.c","w");
    
    if( !file_lexyyc )
    {
        printf("无法打开lex.yy.c文件。\n");
        exit(1);
    }

    if( argc < 2 )
    {
        scanf("%s", fileName);
        if( strlen(fileName) >= 100 )
        {
            printf("字符串长度超过额定限制，退出。\n");
            exit(1);
        }
    }else{
        strcpy( fileName, argv[1] );

    }

    ReadLexFile( fileName, HeaderDef );


//    construct_DFA( newArray, numOfArray );

//    generate_everything( file_lexyyc, newArray, numOfArray, HeaderDef );

//    fclose( file_lexyyc );
    //checkDFA_AND_text( L"#\\s*include\\s*<.+>", L"#include<stdio.h>" );

}
