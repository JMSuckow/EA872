%{
#include "ex3.tab.h"
%}
digito    [0-9]+
frac      [0-9]*\.[0-9]+
hex	  [0-9a-zA-z]+
operador  [-+=]
nl        \n
%%
{digito}	{
		  sscanf(yytext, "%d", &yylval.valor_inteiro);
		  return INTEIRO;
		}
{frac}          {
		  sscanf(yytext, "%f", &yylval.valor_real);
		  return REAL;
		}
{hex}		{
		  sscanf(yytext, "%x", &yylval.valor_hex);
		  return HEX;
		} 
{operador}	return yytext[0];
{nl}		return ('\n');
