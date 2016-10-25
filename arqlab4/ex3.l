%{
#include "ex3.tab.h"
#include <string.h>

%}

WHITESPACE [\t ]+
BREAK_LINE  [\n]
//STRING	[^:\n \r\t]*
STRING	[^,:\n \r\t]*
REQ ^(GET|POST|OPTIONS|HEAD)



%%

{WHITESPACE} return WHITESPACE;

{BREAK_LINE} return BREAK;

// [,](?![^\(]*\))

[,]		{yylval.chr = yytext[0];
		 return VIRGULA;
		}
[:]		{yylval.chr = yytext[0];
		 return DOIS_PONTOS;
		}

[\r]    /* ignore */ ;

^[#].*[\n] /* ignore */ ;

^[.][/].*[\n] /* ignore */ ;

{REQ}     {yylval.str = strdup(yytext);
                return REQUEST;
                }

{STRING}		{yylval.str = strdup(yytext);
                return STRING;
                }



