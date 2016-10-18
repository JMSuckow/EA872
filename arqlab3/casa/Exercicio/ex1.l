%{
#include "ex1.tab.h"
#include <string.h>
%}

TABSPACE [\r\t]+
WHITESPACE [ ]+
OPERATOR  [*]|[-]|[+]|[/]
BREAK_LINE  [\n]
VAR	[a-zA-Z]+[0-9a-zA-z]*
VALOR	([0-9]+)|([0-9]*\.[0-9]+)

%%

{TABSPACE} /* ignore */ ;
{WHITESPACE} /* ignore */ ;
{BREAK_LINE} /* ignore */ ;


"main() {"		{return INICIO_MAIN;}
[}]		{return FIM_MAIN;}

"int"|"float"	{return TIPO;}

[,]		{yylval.chr = yytext[0];
		 return VIRGULA;
		}
[;]		{yylval.chr = yytext[0];
		 return PONTO_VIRGULA;
		}
[=]		{yylval.chr = yytext[0];
		 return IGUAL;
		}
[(]		{yylval.chr = yytext[0];
		 return PARENTESES_ESQ;
		}
[)]		{yylval.chr = yytext[0];
		 return PARENTESES_DIR;
		}

{OPERATOR}	{yylval.chr = yytext[0];
		 return OPERA;
		}
{VAR}		{yylval.str = strdup(yytext);
		 return NOMEVAR;
		}
{VALOR}		{yylval.str = strdup(yytext);
		 return VALOR;
		}
.  ECHO ;


