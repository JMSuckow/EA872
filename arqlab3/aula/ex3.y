%{
#include <stdio.h>
float soma = 0;
%}
%union {
        float 	valor_real;
        int 	valor_inteiro;
	unsigned int valor_hex;
       }
%token <valor_inteiro> INTEIRO 
%token <valor_real> REAL
%token <valor_hex> HEX

%%
linhas:   linha
         | linhas linha
linha:	'\n'		{printf("=%f\n", soma); }
         | exp '\n'	{printf("=%f\n", soma); }
exp   : INTEIRO		{soma += $1; }
        | '+' INTEIRO	{soma += $2; }
        | '-' INTEIRO	{soma -= $2; }
        | '=' INTEIRO	{soma  = $2; }
        | '='		{soma  =  0; }
        | REAL		{soma += $1; }
        | '+' REAL	{soma += $2; }
        | '-' REAL	{soma -= $2; }
        | '=' REAL	{soma  = $2; }
        | HEX          {soma += $1; }
        | '+' HEX      {soma += $2; }
        | '-' HEX      {soma -= $2; }
        | '=' HEX      {soma  = $2; }

%%
void main(){
    yyparse();
}
