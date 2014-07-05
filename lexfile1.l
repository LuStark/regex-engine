%{
    #include <stdio.h>
%}
digit       [0-9]+
space       \s*
newline     \n
INCLUDEFILE    \s*#\s*include\s*<[A-Za-z0-9.]+>
Type        int|void
blockStart  {
blockEnd    }
string      "[^"]*"
zzz_id      [_A-Za-z][_A-Za-z0-9]*
func        {zzz_id}\s*\(\s*\)
dot         ,
semicolon   ;

%%
{ Type } { printf("Type"); }
{ digit } { printf("DIGIT"); }
{ space } { printf(" "); }
{ string } { printf("String"); }
{ newline } { printf("\n"); }
{ INCLUDEFILE } { printf("INCLUDE_FILE"); }
{ blockStart } { printf("BLOCK_Start"); }
{ blockEnd } { printf("BLOCK_End"); }
{ zzz_id   } { printf("ID"); }
{ func  } { printf("Function"); }
{ dot } { printf(","); }
{ semicolon } { printf(";"); }
%%

main()
{ yylex(); return 0; }

int yywrap()
{
    return (1);
}
