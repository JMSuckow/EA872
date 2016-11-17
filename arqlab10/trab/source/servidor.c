#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <unistd.h>	
#include <sys/stat.h>	
#include <sys/types.h>	
#include <fcntl.h>	
#include <errno.h>	
#include <time.h>	
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>


#include "custom_parser.h"
#include "custom_parser.tab.h"

extern FILE* yyin;

static char serverRoot[256];	

#define PATH_OFFSET 20

#define OK	200
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define FORBIDDEN 403
#define NOT_ALLOWED 405
#define NOT_IMPLEMENTED 501
#define SERVICE_UNAVAILABLE 503


static char *resourcePath;

int soquete;			/* soquete para aceitação de pedidos de conexão	*/

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern int yyparse();
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

// volatile sig_atomic_t process_counter = 0;
// volatile sig_atomic_t flag = 0;
// char main_process = 1;

#define OCCUPIED 1
#define NOT_OCCUPIED 0

typedef struct thread_data{
    int novo_soquete;
    char* occupied;
}thread_data;

FILE *logFile;

/* Function setServerRoot()
* Sets the server's root directory
*
* Parameters:
*	char *root: path to root directory
*/

void httpServer_setServerRoot(char *root)
{
	sprintf(serverRoot, "%s", root);
}

/* Function initSocket()
 * Initiate the socket connection given a port number
 *
 * Parameters:
 *	int port: socket's port of communication
 */

void httpServer_initSocket(int port)
{
    struct sockaddr_in servidor;	/* estrutura de informações sobre o servidor	*/
    
    soquete = socket(AF_INET, SOCK_STREAM, 0);
    servidor.sin_family 	= AF_INET;
    servidor.sin_port   	= htons(port);
    servidor.sin_addr.s_addr	= INADDR_ANY;
    
    bind(soquete, (struct sockaddr *)&servidor, sizeof(servidor));
         
    listen(soquete, 5);
}

/* Function buildResponse()
* Build response into a file
*
* Parameters:
*	request: request type code
*	responseCode: permission code to resource
*/
char* buildResponse(HttpRequestList *requestList, char *request, int responseCode){
    
    char *response = (char*)malloc(sizeof(char)*1024);
    int lenght = 0;
    
	lenght += sprintf(response+lenght, "HTTP/1.1 %d ", responseCode);
	switch (responseCode)
	{
		case OK:
			lenght += sprintf(response+lenght, "OK\r\n");
			break;
        case NOT_FOUND:
            lenght += sprintf(response+lenght, "NOT FOUND\r\n");
            break;
        case FORBIDDEN:
            lenght += sprintf(response+lenght, "FORBIDDEN\r\n");
            break;
        case BAD_REQUEST:
            lenght += sprintf(response+lenght, "BAD REQUEST\r\n");
            break;
        case NOT_ALLOWED:
            lenght += sprintf(response+lenght, "NOT ALLOWED\r\n");
            break;
        case NOT_IMPLEMENTED:
            lenght += sprintf(response+lenght, "NOT IMPLEMENTED\r\n");
            break;
        case SERVICE_UNAVAILABLE:
            lenght += sprintf(response+lenght, "SERVICE UNAVAILABLE\r\n");
            break;
	}

    time_t rawtime;
    char buf[80];
    
    time( &rawtime );
    
    strftime(buf,80,"%a, %d %b %Y %H:%M:%S %Z", localtime( &rawtime ));
	lenght += sprintf(response+lenght, "Date: %s\r\n", buf);

	lenght += sprintf(response+lenght, "Server: Servidor HTTP ver 0.3 de Joao Marcos \r\n");

	if(requestList != NULL && httpParser_getParams(CONNECTION) != NULL)
        lenght += sprintf(response+lenght, "Connection: %s\r\n", httpParser_getParams(CONNECTION)->param);
    else
        lenght += sprintf(response+lenght, "Connection: close\r\n");
	
    lenght += sprintf(response+lenght, "Content-Type: text/html\r\n");
    
    if(responseCode == BAD_REQUEST){
        
        lenght += sprintf(response+lenght, "\n\n<html><head>\r\n<title>400 Bad Request</title>\r\n</head><body>\r\n<h1>Bad Request</h1>\r\n</body></html>");
        
    }

    else if(responseCode == SERVICE_UNAVAILABLE){

        lenght += sprintf(response+lenght, "\n\n<html><head>\r\n<title>503 Service Unavailable</title>\r\n</head><body>\r\n<h1>Service Unavailable</h1>\r\n</body></html>");

    }
    else if (strcmp(request,"GET")==0 || strcmp(request,"HEAD")==0 )
	{
		
        
        struct stat statbuff;
        stat(resourcePath, &statbuff);
        
		switch (responseCode)
		{				
			case OK:
                
                lenght += sprintf(response+lenght, "Content-Length: %d\r\n", (int)statbuff.st_size);
                
                strftime(buf, 80, "%a, %d %b %Y %H:%M:%S %Z", localtime(&(statbuff.st_ctime)));

				lenght += sprintf(response+lenght, "Last-Modified: %s\r\n\n", buf);
                
                
                
                if(!strcmp(request,"GET")){
                    
                    char buff[1024];
                    FILE *resource = fopen( resourcePath, "r" );
                    
                    char* buffer = (char*) malloc (sizeof(char)*((int)statbuff.st_size + 1));
                    if (buffer)
                    {
                        fread (buffer, (int)statbuff.st_size +1 , 1, resource);
                        strcat(response,buffer);
                    }
                    fclose(resource);
                }
                
				break;
            case NOT_FOUND:
                
                if(!strcmp(request,"GET")){
                    
                    char string[] = "\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n</body></html>";
                    
                    lenght += sprintf(response+lenght, "Content-Length: %d\r\n",(int) strlen(string));
                    
                    lenght += sprintf(response+lenght, "%s",string);
                }
                
                break;
            case FORBIDDEN:
                
                if(!strcmp(request,"GET")){
                    
                     char string[] = "\n<html><head>\r\n<title>403 Forbidden</title>\r\n</head><body>\r\n<h1>Forbidden</h1>\r\n</body></html>";
                    
                     lenght += sprintf(response+lenght, "Content-Length: %d\r\n",(int) strlen(string));

                     lenght += sprintf(response+lenght, "%s",string);
                    
                }
                
                break;
            default: break;

		}
			
	}
    else if(!strcmp(requestList->cmd, "OPTIONS") || !strcmp(requestList->cmd, "TRACE")
            || !strcmp(requestList->cmd, "POST") || !strcmp(requestList->cmd, "DELETE")){
        
        char string[] = "\n<html><head>\r\n<title>405 Method Not Allowed</title>\r\n</head><body>\r\n<h1>Method Not Allowed</h1>\r\n</body></html>";
        
        lenght += sprintf(response+lenght, "Content-Length: %d\r\n",(int) strlen(string));
        
        lenght += sprintf(response+lenght, "%s",string);
        
    }
    
    else if(responseCode == NOT_IMPLEMENTED){
        
        char string[] = "\n<html><head>\r\n<title>501 Method Not Implemented</title>\r\n</head><body>\r\n<h1>Method Not Implemented</h1>\r\n</body></html>";
        
        lenght += sprintf(response+lenght, "Content-Length: %d\r\n", (int) strlen(string));
        
        lenght += sprintf(response+lenght, "%s",string);
        
    }
    
    return response;

}

/* Function testResource()
 * Verify if a resource on a given path exists and if it is accessable
 *
 */
int testResource(char *serverRoot, char *resource)
{
    int resourceSize = strlen(serverRoot) + strlen(resource) + PATH_OFFSET;
    resourcePath = (char *)malloc(resourceSize*sizeof(char));
    sprintf(resourcePath, "%s%s", serverRoot, resource);
    
    struct stat fileStat;
    
    
    //Confere se o path não acessa níveis acima da raiz do server
    char* token = strtok(resource, "/");
    
    char* tmp = (char *)malloc(resourceSize*sizeof(char));
    char* rootUp = (char *)malloc((strlen(serverRoot)+5)*sizeof(char));
    sprintf(tmp, "%s/%s", serverRoot, token);
    sprintf(rootUp, "%s/../", serverRoot);
    struct stat dirStat;
    
    stat(rootUp,&dirStat);
    free(rootUp);
    
    while( token != NULL )
    {
        if(stat(tmp,&fileStat) < 0){
            free(tmp);
            return NOT_FOUND;
        }else{
            if(dirStat.st_ino == fileStat.st_ino){
                free(tmp);
                return FORBIDDEN;
            }
        }
        
        token = strtok(NULL, "/");
        sprintf(tmp, "%s/%s", tmp, token);
    }
    
    int file,dir, i;
    
    //Adquire os dados do arquivo
    if(stat(resourcePath,&fileStat) < 0){
        return NOT_FOUND;
    }
    
    //Verifica permissao de leitura
    if (!(fileStat.st_mode & S_IRUSR)){
        return FORBIDDEN;
    }
    
    if(!(S_ISDIR(fileStat.st_mode))){
        //ARQUIVO
        return OK;
    }else{
        //DIRETORIO
        //Verifica permissao de varredura
        if (!((fileStat.st_mode & S_IXUSR))){
            return FORBIDDEN;
        }
        else{
            char *strIndex = malloc(sizeof(char)*(strlen(resourcePath)+12));
            char *strWelcome = malloc(sizeof(char)*(strlen(resourcePath)+14));;
            struct stat indexStat, welcomeStat;
            
            strcpy(strIndex,resourcePath); strcat(strIndex,"/index.html");
            strcpy(strWelcome,resourcePath); strcat(strWelcome,"/welcome.html");
            
            //Verifica existencia de index.html e welcome.html
            if(stat(strIndex,&indexStat) < 0 && stat(strWelcome,&welcomeStat)){
                return NOT_FOUND;
            }
            
            //Verifica a permissao de leitura de index.html e welcome.html
            if (!(indexStat.st_mode & S_IRUSR) && !(welcomeStat.st_mode & S_IRUSR)){
                return FORBIDDEN;
            }
            
            if (indexStat.st_mode & S_IRUSR){
                resourcePath = (char *)realloc(resourcePath,(strlen(strIndex)+1)*sizeof(char));
                strcpy(resourcePath,strIndex);
                return OK;
            }else if (welcomeStat.st_mode & S_IRUSR){
                resourcePath = (char *)realloc(resourcePath,(strlen(strWelcome)+1)*sizeof(char));
                strcpy(resourcePath,strWelcome);
                return OK;
            }
        }
        
    }
    
    return -1;
    
}

/* Function server_answerRequest()
* Answer a given request
*
* Parameters:
*	requestList: request to be answered
*/
char* httpServer_answerRequest(HttpRequestList *requestList)
{	
	int code;
    
    char* response;
    
    if(requestList != NULL){
        
        if (!strcmp(requestList->cmd, "GET")){
            code = testResource(serverRoot, requestList->params->param);
        }else if (!strcmp(requestList->cmd, "HEAD")){
            code = testResource(serverRoot, requestList->params->param);
        }else if(!strcmp(requestList->cmd, "OPTIONS") || !strcmp(requestList->cmd, "TRACE")
             || !strcmp(requestList->cmd, "POST") || !strcmp(requestList->cmd, "DELETE")){
            code = NOT_ALLOWED;
        }else{
            code = NOT_IMPLEMENTED;
        }

        response = buildResponse(requestList, requestList->cmd, code);
    }else{
        code = BAD_REQUEST;
        response = buildResponse(NULL, "", code);
    }

    
    return response;

}

/* Function addToLog()
* Adds request/response to file logFile
*
* Parameters:
*	logFile : log file
*	request: request string
*	response: response string
*/

void httpServer_addToLog(FILE* logFile, char* request, char* response)
{

    fflush(logFile);
	fprintf(logFile, "--- REQUEST ---\r\n\r\n");

    fprintf(logFile, "%s", request);


    fprintf(logFile, "\r\n--- RESPONSE ---\r\n\r\n");
    
    int lenght = strlen(response);
    int i, limit = 0;
    for(i = 0; i < lenght - 1; i++){
        if(response[i] == '\n' && response[i + 1] == '\n'){
            limit = i+1; break;
        }
    }
    if(limit == lenght-1){
        fprintf(logFile, "%s", response);
    }else{
        response[limit] = 0;
        fprintf(logFile, "%s", response);
    }
	
	fprintf(logFile,"\n********************************************************************\r\n");
}

/* Function process_request()
* Process a request and builds a response to it. 
*
* Parameters:
*   data : data passed for the thread to process a socket connection
*/
void* process_request(void* arg){

    thread_data* data = (thread_data*) arg;

    int novo_soquete = data->novo_soquete;

    char* flag_address = data->occupied;

    fd_set fdset;
    struct timeval tv;
    
    int socket_value = 1;

    char area[1024];  

    while(1){
        int i;
        for(i=0; i < 1024; i++)
            area[i] = 0;        

        if(socket_value == 1){

            socket_value = 0;

            int mensagem_compr = recv(novo_soquete, area, sizeof(area), 0);
        
             // printf("%s",area);

            HttpRequestList *requestList;
            char* response;

            YY_BUFFER_STATE str_buffer = yy_scan_string(area);
            
            // now all scanning and parsing
            // call parser
            httpParser_cleanRequestList();
            
            if(!yyparse()){
                requestList = httpParser_getRequestList();
                response = httpServer_answerRequest(requestList);
                
                write(novo_soquete, response, strlen(response)+1);

                // printf("%s",response);

            }
            else{ // ERROR Bad Request
                response = httpServer_answerRequest(NULL);

                write(novo_soquete, response, strlen(response)+1);

                close(novo_soquete);
                *(flag_address) = NOT_OCCUPIED;
                free(data);
                pthread_exit(NULL);
            }
            
            // sleep(5);

            yy_delete_buffer(str_buffer); // free up memory

            if(requestList != NULL && httpParser_getParams(CONNECTION) != NULL){
                if(!strcmp(httpParser_getParams(CONNECTION)->param, "close")){
                    close(novo_soquete);
                    *(flag_address) = NOT_OCCUPIED;
                    free(data);
                    pthread_exit(NULL);
                }

            }
            
            char* request = (char*)malloc(sizeof(char)*1024);
            request = strdup(area);
            
            httpServer_addToLog(logFile, request, response);
            
            if(response != NULL)
                free(response);


            /* Mantains connection alive until a timeout of 10s without connectivity is reached */

            FD_ZERO(&fdset);
            FD_SET(novo_soquete, &fdset);
            tv.tv_sec = 10;             /* 10 segundos timeout */
            tv.tv_usec = 0;

            do{
                errno = 0;
                socket_value = select(novo_soquete + 1, &fdset, NULL, NULL, &tv);

                // printf("%d ->SOQUETE %d\n",socket_value,errno);

                if(socket_value < 0)
                    continue;

                if(socket_value == 0){
                    close(novo_soquete);
                    *(flag_address) = NOT_OCCUPIED;
                    free(data);
                    pthread_exit(NULL);
                }

            }while(errno==EINTR);

        }

    }

}

char* find_next_unnocupied_thread(char* thread_flags, int N){
    int next_unnocupied_thread = 0;
    int i;
    for(i = 0; i < N; i++)
        if(thread_flags[i] == NOT_OCCUPIED){
            return &(thread_flags[i]);
        }

    return NULL;
}

int count_active_threads(char* thread_flags, int N){
    int active_threads = 0;
    int i;
    for(i = 0; i < N; i++)
        active_threads += thread_flags[i];

    return active_threads;
}

/* Function onExit()
* Close the socket when the program is terminated 
*/
static void onExit(void){
    close(soquete);
    fclose(logFile);
}

int main(int argc, char **argv)
{ 

    int port = atoi(argv[1]);

    int N = atoi(argv[4]);

    int i;

    char thread_flags[N];

    for(i = 0; i < N; i++)
        thread_flags[i] = NOT_OCCUPIED;
    
	// prepare webspace 
	httpServer_setServerRoot(argv[2]);

	logFile = fopen(argv[3], "a");

    httpServer_initSocket(port);
    
    //Set up function onExit
    atexit(onExit);


    struct sockaddr_in cliente;		/* estrutura de informações sobre os clientes	*/
    int novo_soquete;			/* soquete de conexão aos clientes		*/
    int nome_compr;			/* comprimento do nome de um cliente		*/   


    // Set up a timout of 15s without connectivity 
    fd_set fdset;
    struct timeval tv;
    
    FD_ZERO(&fdset);
    FD_SET(soquete, &fdset);
    tv.tv_sec = 15;             /* 15 segundos timeout */
    tv.tv_usec = 0;

    while (1) {

        int socket_value;

        do{
            errno = 0;
            socket_value = select(soquete+1, &fdset, NULL, NULL, &tv);

        }while(errno==EINTR);

        int thread_count = count_active_threads(thread_flags, N);

       // printf("%d", thread_count);

        if(socket_value < 0){   // Error during select  
            continue;
        }
        else if(socket_value == 0 && thread_count == 0){     //No socket for reading

            exit(0);

        }
        else if(socket_value == 0 && thread_count != 0){     //No socket for reading but thread connections are still open

            continue;

        }
        else{   //Exists a new connection

            nome_compr = sizeof(cliente);
            novo_soquete = accept(soquete, (struct sockaddr *)&cliente, &nome_compr);

            if(novo_soquete <= 0){
                continue;
            }

            if(thread_count < N){

                pthread_t id;

                thread_data* data = (thread_data*) malloc(sizeof(thread_data));

                data->novo_soquete = novo_soquete;
                data->occupied = find_next_unnocupied_thread(thread_flags, N);
                *(data->occupied) = OCCUPIED;

                pthread_create(&id, NULL, &process_request, (void*) data);

            }else{  //Reached server's limit of connections
                char* response = buildResponse(NULL, "", SERVICE_UNAVAILABLE);
                write(novo_soquete, response, strlen(response)+1);
                close(novo_soquete);

            }

        }

            

        
    }  // Laço termina aqui
    
    
    
    
	
}
