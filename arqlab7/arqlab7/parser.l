%{				
#include "parser.tab.h"				
%}				
				
%%				
				
"GET "		return GET;							
"HTTP/1.1"	return VERSION11;	
				
"Host: "	return	HOST;
				
\n		return	NL;
\r		return CR;
\n\n		return END;		
\n\r\n		return END;		
,		return COMMA;		
[/]		return SLASH;		
[ ]		return BLANK;		
\t		return BLANK;			
.		{yylval.text = yytext[0]; return CHARACTER;}
				
%%				
