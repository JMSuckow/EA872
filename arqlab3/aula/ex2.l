%{
	#include "ex2.tab.h"	// Este include file é gerado pelo bison. Por isso, 
				// compile o analisador sintático antes do léxico.
%}
%%
[0-9a-fA-F]+	{
		sscanf(yytext, "%x", &yylval);
		printf("Léxico: yytext (texto) = %s \t yyval (valor) =  %d\n", yytext, yylval);
		return (INTEIRO);
	}
\n	return ('\n');
%%
