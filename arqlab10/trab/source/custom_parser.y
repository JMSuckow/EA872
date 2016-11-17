%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "custom_parser.h"


HttpRequestList *requestList;
HttpRequestList *last_request;

extern char *yytext;

void add_cmd(char* name, char* params);
void add_param(char *str);


%}
%union{
	char *str;
	char chr;
}

%error-verbose

%token <chr> DOIS_PONTOS
%token <chr> WHITESPACE
%token <chr> VIRGULA
%token <str> STRING
%token <str> REQUEST
%token <chr> BREAK

%type <str> entrada comando texto input

%%

input   : entrada input
    |   entrada

entrada:   comando DOIS_PONTOS texto BREAK{add_cmd($1,$3);}
    |       DOIS_PONTOS texto  BREAK {printf("Erro: Sintaxe inexistente\n");}
    |       comando DOIS_PONTOS BREAK{add_cmd($1,NULL);}
    |       REQUEST WHITESPACE STRING WHITESPACE STRING BREAK
            {char* string =malloc(sizeof(char)*(strlen($3)+strlen($5)+2));
                strcpy(string,$3); strcat(string,"~"); strcat(string,$5);
                add_cmd($1,string);}
    |        STRING WHITESPACE STRING WHITESPACE STRING BREAK
            {char* string =malloc(sizeof(char)*(strlen($3)+strlen($5)+2));
                strcpy(string,$3); strcat(string,"~"); strcat(string,$5);
                add_cmd($1,string);}
    |       BREAK   {$$ = "\n";}
;

comando :   STRING WHITESPACE comando {char* string =malloc(sizeof(char)*(strlen($1)+strlen($3)+2));
                                        strcpy(string,$1); strcat(string," "); strcat(string,$3);
                                        $$=string;}
|       WHITESPACE comando {$$ = $2;}
|       STRING  {$$ = $1;}
;

texto:  WHITESPACE texto    {$$ = $2;}
    |   STRING VIRGULA texto {char* string =malloc(sizeof(char)*(strlen($1)+strlen($3)+2));
                                strcpy(string,$1); strcat(string,"~"); strcat(string,$3);
                                $$=string;}
    |   STRING WHITESPACE texto {char* string =malloc(sizeof(char)*(strlen($1)+strlen($3)+2));
                                strcpy(string,$1); strcat(string," "); strcat(string,$3);
                                $$=string;}
    |   STRING DOIS_PONTOS texto {char* string =malloc(sizeof(char)*(strlen($1)+strlen($3)+2));
                                    strcpy(string,$1); strcat(string,":"); strcat(string,$3);
                                    $$=string;}
    |   STRING WHITESPACE {$$ = $1;}
    |   STRING {$$ = $1;}
;

%%
/*int main(){
    
    requestList=NULL;
    last_request=NULL;
    
    yyparse();
    
    return 0;
}*/

void add_cmd(char* name, char* params){
    
    HttpRequestList* cmd = (HttpRequestList*) malloc(sizeof(HttpRequestList));
    cmd->params = NULL;
    cmd->used = 0;
    cmd->size = 10;
    cmd->cmd = strdup(name);
    cmd->next = NULL;
    
    if(last_request == NULL){
        requestList = cmd;
        last_request = requestList;
    }else{
        last_request->next = cmd;
        last_request = cmd;
        
    }
    
    //adicionar parametros
    
    Params* last_param;
    last_param = cmd->params;
    
    char* pch = strtok (params,"~");
    while (pch != NULL)
    {
        
        Params* p = (Params*) malloc(sizeof(Params));
        p->param = strdup(pch);
        p->next = NULL;
        
        if(last_param == NULL){
            last_param=p;
            last_request->params = p;
        }else{
            last_param->next = p;
            last_param = p;
        }

        pch = strtok (NULL, "~");
    }
    
}

/* printRequestList()
 * Imprime todas informacoes de uma lista de requisicoes
 *
 * Parametros:
 *  HttpRequest *requestList: ponteiro para um lista de requisicoes
 */

void  httpParser_printRequestList(HttpRequestList *requestList)
{
    HttpRequestList* cmd = requestList;
    
    while(cmd != NULL){
        
        printf("Comando: %s\n", cmd->cmd);
        
        Params* params = cmd->params;
        
        while (params != NULL){
            
            printf("\tParametro %s\n", params->param);
            params = params->next;
        }
        
        cmd = cmd->next;
    }
}

/* Funcao getResource(HttpRequestList *requestList)
 * Retorna a string do recurso requisitado
 *
 * Parametros:
 * HttpRequest *requestList: ponteiro para um lista de requisicoes
 */
char* httpParser_getResource(HttpRequestList *requestList){
    
    HttpRequestList* cmd = requestList;
        
    Params* params = cmd->params;
        
    return params->param;
    
}

/* Funcao getVersion(HttpRequestList *requestList)
 * Retorna a string da versao requisitada
 *
 * Parametros:
 * HttpRequest *requestList: ponteiro para um lista de requisicoes
 */
char* httpParser_getVersion(HttpRequestList *requestList){
    
    HttpRequestList* cmd = requestList;
    
    Params* params = cmd->params;
    
    return params->next->param;
    
}

/* Funcao getParams(HttpCmds cmd)
 * Retorna um ponteiro para a lista de paramtros de um comando
 *
 * Parametros:
 * HttpCmds cmd: comando desejado
 */
Params* httpParser_getParams(HttpCmds cmd){

    if(cmd == TYPE){
        
        return requestList->params;
        
    }else{
    
        const char* string;
    
        switch(cmd){
            case HOST: string = HOST_STRING;break;
            case CONNECTION: string = CONNECTION_STRING;break;
            case UPGRADE_INSECURE_REQUESTS: string = UPGRADE_INSECURE_REQUESTS_STRING;break;
            case USER_AGENT: string = HOST_STRING;break;
            case ACCEPT: string = USER_AGENT_STRING;break;
            case ACCEPT_ENCODING: string = ACCEPT_ENCODING_STRING;break;
            case ACCEPT_LANGUAGE: string = ACCEPT_LANGUAGE_STRING;break;
            default: break;
        }
        

	HttpRequestList* cmd = NULL;
	if(requestList != NULL)
        	cmd = requestList->next;
        
        while(cmd != NULL){
            
            if(strcmp(cmd->cmd,string) == 0){
                
                return cmd->params;
                
            }
            cmd = cmd->next;
        }
    
    }
    
    return NULL;
    
}

/* Funcao getRequestList()
 * Retorna o ponteiro para a lista de requisicoes
 */

HttpRequestList* httpParser_getRequestList()
{
    return requestList;
}

/* Funcao cleanRequestList()
 * Limpa a lista de requisicoes
 */

void httpParser_cleanRequestList()
{
    HttpRequestList* cmd = requestList;
    
    while(cmd != NULL){
        
        Params* params = cmd->params;
        
        while (params != NULL){
            
            Params* aux = params;
            params = params->next;
            free(aux);
        }
        HttpRequestList* temp = cmd;
        cmd = cmd->next;
        free(temp);
    }
    
    requestList = NULL;
    last_request = NULL;
}
