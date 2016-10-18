%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct par{
    struct par *next;
    char    *param;
} Params;

typedef struct cmds{
    struct cmds *next;
    char    *cmd;
    Params    *params;
    size_t used;
    size_t size;
} Comandos;

Comandos *cmds;
Comandos *last_cmd;


void add_cmd(char* name, char* params);
void add_param(char *str);
void imprime();

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
int main(){
    
    cmds=NULL;
    last_cmd=NULL;
    
    
    
    yyparse();
    
    imprime();
    
    return 0;
}

void imprime(){
    
    
    Comandos* cmd = cmds;
    
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

void add_cmd(char* name, char* params){
    
    Comandos* cmd = (Comandos*) malloc(sizeof(Comandos));
    cmd->params = NULL;
    cmd->used = 0;
    cmd->size = 10;
    cmd->cmd = strdup(name);
    cmd->next = NULL;
    
    if(last_cmd == NULL){
        cmds = cmd;
        last_cmd = cmds;
    }else{
        last_cmd->next = cmd;
        last_cmd = cmd;
        
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
            last_cmd->params = p;
        }else{
            last_param->next = p;
            last_param = p;
        }

        pch = strtok (NULL, "~");
    }
    
}
