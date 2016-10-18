%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parser.h"

static HttpRequest *requestList;
static char *pch2;
static char tmpValue[1024];
static int tmpIndex = 0;

extern char *yytext;

/*int main(int argc, char *argv[]) {
	if(!yyparse())
	{
		requestList = httpParser_getRequestList();
		httpParser_printRequestList(requestList);
	}
	return 0;
}*/

%}

%union {
char text; 
} 

%token NL CR COMMA END SLASH BLANK

%token GET//HEAD,OPTIONS, etc
%token VERSION11
%token HOST //ACCEPT, ACCEPT_ENCODING, etc
%token <text> CHARACTER

%%				
http_message: request	parameters;	/* what an HTTP message looks like */		
				
request: reqtype resource version CR NL	;/* the first	line of an HTTP message */
				
reqtype: 	GET		{ 
				requestList = (HttpRequest *)malloc(sizeof(HttpRequest));
 				requestList->type = strdup("GET");}			
		;				
								
resource: SLASH chars;
				
chars: /* nothing */		
	| chars char 
	| chars slash char 
	; 
		
slash: SLASH {tmpValue[tmpIndex++] = '/';}	
	
char: CHARACTER {tmpValue[tmpIndex++] = yyval.text;}	
	| BLANK {finish_resource(); 
		requestList->resource = pch2;}	
	
version: VERSION11 {requestList->version = strdup("1.1");}	
	
parameters: /* no parameter */	
	| parameters parameter	
	;	
	
parameter: partype fields	/* remaining parameters */
	;	
	
partype: HOST	{}
	;				
		
fields:  /* nothing */		
	| fields field 	
	; 

field: CHARACTER {}
	| SLASH	{}	
	| BLANK	{}	
	| COMMA	{}	
	| NL	{}	
	| CR	{}	
	| END	{}	
	;	
	
	
%%	
	
/* Funcao finish_resource()
* Auxilia na criacao de um novo recurso. Ao determinar todo o recurso que sera usado 
* pela requisicao, adiciona uma barra no inicio do mesmo e guarda uma referencia em 

* um ponteiro auxiliar. 
*/ 
finish_resource() 				
{ 				
	int i,j=0; 				
	char *temp; 				
							
	temp = (char *) strdup(tmpValue);
				
	for(i = 0; i < tmpIndex; i++) tmpValue[i + 1] = temp[j++];
	tmpValue[0] = '/';
	tmpValue[tmpIndex+1] = '\0';
	tmpIndex = 0;
				
	pch2 = strdup(tmpValue);
								
	free(temp); 							
}

/* printRequestList()
* Imprime todas informacoes de uma lista de requisicoes 
* 
* Parametros: 
*  HttpRequest *requestList: ponteiro para um lista de requisicoes 
*/ 

void  httpParser_printRequestList(HttpRequest *requestList) 
{ 
	HttpRequest *req; 
	req = requestList; 

	printf("Request: %s\n", req->type); 
	printf("Resource: %s\n", req->resource); 
	printf("Version: %s\n", req->version); 
} 				

/* Funcao getRequestList()
* Retorna o ponteiro para a lista de requisicoes 
*/ 

HttpRequest* httpParser_getRequestList() 
{ 
	return requestList; 
} 
