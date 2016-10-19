#ifndef PARSER_H
#define PARSER_H

/*typedef struct HttpRequest_ {
	char *type;
	char *resource;
	char *version;
} HttpRequest;*/


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
} HttpRequestList;

typedef enum    {TYPE,
                HOST,
                CONNECTION,
                UPGRADE_INSECURE_REQUESTS,
                USER_AGENT,
                ACCEPT,
                ACCEPT_ENCODING,
                ACCEPT_LANGUAGE} HttpCmds;

static const char *TYPE1_STRING = "GET";
static const char *TYPE2_STRING = "HEAD";
static const char *HOST_STRING = "Host";
static const char *CONNECTION_STRING = "Connection";
static const char *UPGRADE_INSECURE_REQUESTS_STRING = "Upgrade-Insecure-Requests";
static const char *USER_AGENT_STRING = "User-Agent";
static const char *ACCEPT_STRING = "Accept";
static const char *ACCEPT_ENCODING_STRING = "Accept-Encoding";
static const char *ACCEPT_LANGUAGE_STRING = "Accept-Language";


/* Funcao getRequestList()
* Retorna o ponteiro para a lista de requisicoes
*/
HttpRequestList *httpParser_getRequestList();

/* Funcao getResource(HttpRequestList *requestList)
 * Retorna a string do recurso requisitado
 *
 * Parametros:
 * HttpRequest *requestList: ponteiro para um lista de requisicoes
 */
char* httpParser_getResource(HttpRequestList *requestList);

/* Funcao getVersion(HttpRequestList *requestList)
 * Retorna a string da versao requisitada
 *
 * Parametros:
 * HttpRequest *requestList: ponteiro para um lista de requisicoes
 */
char* httpParser_getVersion(HttpRequestList *requestList);

/* Funcao getParams(HttpCmds cmd)
 * Retorna um ponteiro para a lista de paramtros de um comando
 *
 * Parametros:
 * HttpCmds cmd: comando desejado
 */
Params* httpParser_getParams(HttpCmds cmd);

/* printRequestList()
* Imprime todas informacoes de uma lista de requisicoes
*
* Parametros:
* HttpRequest *requestList: ponteiro para um lista de requisicoes
*/
void httpParser_printRequestList(HttpRequestList *requestList);

#endif
