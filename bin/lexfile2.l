%{
    #include <stdio.h>
%}
noabc [^a][^b][^c]

%%
{noabc} { printf("NOABC\n"); }
%%

main()
{ yylex(); return 0; }

int yywrap()
{
    return (1);
}
