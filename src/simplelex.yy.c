#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


int     numOfRegex = 2;

int     matchlength_for_regex[numOfRegex];

static int FinalStatus_digit[]=
{0, 1 }; 

static int T_digit[][128]=
{
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

int
Recognize_for_digit( char * text )
{
    int match_length, has_matched, t;
    char *p, c;
    match_length = 0;

    t = 0;
    p = text;
    has_matched = 0;
    while( (*p)!='\0' )
    { 
        t = T_digit[t][*p];
        if( t == -1 )
        {
            if( has_matched )
            {
                return match_length;   
            }
            else
                return -1;
        }
        else
        {
            match_length++;
            if( FinalStatus_digit[t]==true )
                has_matched = 1;
        }
    }                            
    if( FinalStatus_digit[t]==true )             
        return match_length;             
    return -1;                
}                                

int RecognizeAndAct_for_digit( )
{
    if( true )
    {
        printf("DIGIT"); 
        return 1;
    }
    return 0;
}

static int FinalStatus_letter[]=
{0, 1 }; 

static int T_letter[][128]=
{
		{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, -1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1},
{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

int Recognize_for_letter( char * text)
{
    int match_length, has_matched, t;
    char *p;
    match_length = 0;

    t = 0;
    p = text;
    has_matched = 0;
    while( (*p)!='\0' )
    { 
        t = T_letter[t][*p];
        if( t == -1 )
        {
            if( has_matched )
            {
                return match_length;   
            }
            else
                return -1;
        }
        else
        {
            match_length++;
            if( FinalStatus_letter[t]==true )
                has_matched = 1;
        }
    }                            
    if( FinalStatus_letter[t]==true )             
        return match_length;             
    return -1;      
}                                

int RecognizeAndAct_for_letter( )
{
    if( true )
    {
        printf("LETTER"); 
        return 1;
    }
    return 0;
}

int (*ptr[numOfRegex])( char* );
int (*ptr_action[numOfRegex])( );

void set_functions_for_allfunctions( )
{
    ptr[0] = Recognize_for_digit;
    ptr[1] = Recognize_for_letter;
    
    ptr_action[0] = RecognizeAndAct_for_digit;
    ptr_action[1] = RecognizeAndAct_for_letter;
}

int FindMaxIndex( int A[], int n )
{
    int max_i;
    int max;

    int i;

    max = -1;
    max_i = 0;
    for( i=0; i<n; i++ )
    {
        if( A[i]>max )
        {
            max = A[i];
            max_i = i;
        }
    }
    return max_i;
}

void scan(char *filename)
{
    char    Program[3000];
    FILE    *f;
    int     i = 0, c, scanPos, regexID, matchLen;

    f = fopen(filename, "r");
    if( !f ){
        printf("文件无法打开。\n");
        exit(1);
    }
    while( (c=fgetc(f)) != EOF )
    {
        if( c=='\n' )
        {
            c = fgetc(f);
            if( c!=EOF )
                ungetc(c,f);
            else
                break;
        }
        Program[i++] = c;
    }
    Program[i++] = '\0';

    scanPos = 0;
    while( Program[scanPos]!='\0' )
    {
        for( i = 0; i < numOfRegex; i++ )
        {   
            matchlength_for_regex[i] = ptr[i]( Program + scanPos );
        }
        regexID = FindMaxIndex( matchlength_for_regex, numOfRegex );
        if( (matchLen = matchlength_for_regex[regexID]) > 0 )
        {
            ptr_action[regexID]();
        }else{
            printf("无法向下分析.\n");
            exit(1);
        }
        scanPos += matchLen;
    }
}

int main(int argc, char *argv[])
{
    set_functions_for_allfunctions();
        
    scan("program.txt");

    printf("\n");
}
