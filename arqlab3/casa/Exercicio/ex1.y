%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char **array;
    size_t used;
    size_t size;
} Var;

Var variables;

void verify_usage(char *str);
void add_var(char *str);

%}
%union{
	char *str;
	char chr;
}

%error-verbose

%token INICIO_MAIN
%token FIM_MAIN
%token <str> TIPO
%token <chr> PONTO_VIRGULA
%token <chr> VIRGULA
%token <str> NOMEVAR
%token <chr> IGUAL
%token <str> VALOR
%token <str> OPERA
%token <chr>PARENTESES_ESQ
%token <chr> PARENTESES_DIR


%%

prog_fonte 	:	INICIO_MAIN conteudo_prog FIM_MAIN
               ;

conteudo_prog  :	declaracoes expressoes
               ;

declaracoes    :	linha_declara declaracoes
            |	linha_declara
            ;

linha_declara 	: 	TIPO variaveis PONTO_VIRGULA 
            ;

variaveis 	:	identificador VIRGULA variaveis
            |	identificador
               ;

identificador 	:	NOMEVAR
                |	NOMEVAR IGUAL VALOR 	{add_var($1); }
               ;			    
					   


expressoes 	:	linha_executavel expressoes
            |	linha_executavel
               ;

linha_executavel	:	NOMEVAR IGUAL operacoes PONTO_VIRGULA {add_var($1); }
               		;


operacoes 	: 	operacoes OPERA operacoes
		|    PARENTESES_ESQ operacoes PARENTESES_DIR
        | 	VALOR
        | 	NOMEVAR	{verify_usage($1);}
;

%%
int main(){
    
    variables.array = (char **)malloc(10 * sizeof(char*));
    variables.used = 0;
    variables.size = 10;
    
    yyparse();
    
    return 0;
}


void verify_usage(char *str){
    int i;
    for(i=0; i<variables.used; i++){
        
        if(strcmp(str,variables.array[i])==0)
            break;
        
    }
    
    if(i==variables.used){
        printf("Warning: Variável %s não inicializada.\n", str);
    }
    
}

void add_var(char *str){
    if (variables.used == variables.size) {
        variables.size *= 2;
        variables.array = (char **)realloc(variables.array, variables.size * sizeof(char*));
    }
    variables.array[variables.used++] = str;
}
