%{
    #include <stdio.h>
%}
reserved else|if|int|return|void|for|while
letter  [A-Za-z]
digit   [0-9]
IF  if
identify [_a-zA-Z]([a-zA-Z]|[0-9])*
signedNat   (\+|-)?[0-9]+
number (\+|-)?[0-9]+(\.[0-9]+)?(E(\+|-)?[0-9]+)?
float (\+|-)?[0-9]+(\.[0-9]+)?f

%%
{digit} { printf("DIGIT"); }
{reserved} { printf("RESERVED"); }
{IF}   { printf("IF"); }
{signedNat} { printf("SIGNED_NAT"); }
{identify} { printf("IDENTIFY"); }
{number} { printf("NUMBER"); }
{float} { printf("FLOAT"); }

%%

main()
{ yylex(); return 0; }

int yywrap()
{
    return (1);
}
