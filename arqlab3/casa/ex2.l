%{
#include "ex2.tab.h"
#include <string.h>

%}

WHITESPACE [\r\t ]+
BREAK_LINE  [\n]
STRING	[^,:\n \r\t]*



%%

{WHITESPACE} return WHITESPACE;

{BREAK_LINE} return BREAK;

[,]		{yylval.chr = yytext[0];
		 return VIRGULA;
		}
[:]		{yylval.chr = yytext[0];
		 return DOIS_PONTOS;
		}

^[#].*[\n] /* ignore */ ;

{STRING}		{yylval.str = strdup(yytext);
                return STRING;
                }



