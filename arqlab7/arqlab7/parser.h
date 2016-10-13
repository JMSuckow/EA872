#ifndef PARSER_H
#define PARSER_H

typedef struct HttpRequest_ {
	char *type;
	char *resource;
	char *version;
} HttpRequest;

/* Funcao getRequestList()
* Retorna o ponteiro para a lista de requisicoes
*/
HttpRequest *httpParser_getRequestList();

/* printRequestList()
* Imprime todas informacoes de uma lista de requisicoes
*
* Parametros:
* HttpRequest *requestList: ponteiro para um lista de requisicoes
*/
void httpParser_printRequestList(HttpRequest *requestList);

#endif
