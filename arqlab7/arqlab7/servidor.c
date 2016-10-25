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


static char *resourcePath;

//Funcao para ler e imprimir na saida padrao do sistema o conteúdo do arquivo
void read_and_print_file(char* file_string, struct stat fileStat){
    
    int file;
    if((file = open(file_string, O_RDONLY, 0600)) == -1)
    {
        printf("Erro na abertura de arquivo\n");
        exit(errno);
    }
    
    char buf[fileStat.st_size];
    if (read(file, buf, sizeof(buf)) == -1){
        printf("Erro na leitura do arquivo");
        exit(errno);
    }
    if (write(STDOUT_FILENO, buf, fileStat.st_size) == -1){
        printf("Erro na escrita");
        exit(errno);
    }
    
    close(file);
    
}



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

/* Function buildResponse()
* Build response into a file
*
* Parameters:
*	request: request type code
*	responseCode: permission code to resource
* 	requestFile: original request file
* 	responseFile: build response into it
*/
void buildResponse(HttpRequestList *requestList, char *request, int responseCode, FILE* requestFile, FILE* responseFile)
{
    
	fprintf(responseFile, "HTTP/1.1 %d ", responseCode);
	switch (responseCode)
	{
		case OK:
			fprintf(responseFile, "OK\r\n");
			break;
        case NOT_FOUND:
            fprintf(responseFile, "NOT FOUND\r\n");
            break;
        case FORBIDDEN:
            fprintf(responseFile, "FORBIDDEN\r\n");
            break;
        case BAD_REQUEST:
            fprintf(responseFile, "BAD REQUEST\r\n");
            break;
        case NOT_ALLOWED:
            fprintf(responseFile, "NOT ALLOWED\r\n");
            break;
        case NOT_IMPLEMENTED:
            fprintf(responseFile, "NOT IMPLEMENTED\r\n");
            break;
	}

    time_t rawtime;
    char buf[80];
    
    time( &rawtime );
    
    strftime(buf,80,"%a, %d %b %Y %H:%M:%S %Z", localtime( &rawtime ));
	fprintf(responseFile, "Date: %s\r\n", buf);

	fprintf(responseFile, "Server: Servidor HTTP ver 0.1 de Joao Marcos \r\n");

	if(requestList != NULL)
        fprintf(responseFile, "Connection: %s\r\n", httpParser_getParams(CONNECTION)->param);
    else
        fprintf(responseFile, "Connection: close\r\n");
	
    fprintf(responseFile, "Content-Type: text/html\r\n");
    
    if(responseCode == BAD_REQUEST){
        
        fprintf(responseFile, "\n\n<html><head>\r\n<title>400 Bad Request</title>\r\n</head><body>\r\n<h1>Bad Request</h1>\r\n</body></html>");
        
    }
    else if (strcmp(request,"GET")==0 || strcmp(request,"HEAD")==0 )
	{
		
        
        struct stat statbuff;
        stat(resourcePath, &statbuff);
        
        fprintf(responseFile, "Content-Length: %d\r\n", (int)statbuff.st_size);
        
		switch (responseCode)
		{				
			case OK:
                
                strftime(buf, 80, "%a, %d %b %Y %H:%M:%S %Z", localtime(&(statbuff.st_ctime)));

				fprintf(responseFile, "Last-Modified: %s\r\n\n", buf);
                
                if(!strcmp(request,"GET")){
                    
                    int fid;
                    fid = open(resourcePath, O_RDONLY);
                    read(fid, resourcePath, statbuff.st_size);
                    fwrite(resourcePath, sizeof(char), statbuff.st_size, responseFile);
                    close(fid);
                }
                
				break;
            case NOT_FOUND:
                
                if(!strcmp(request,"GET")){
                    
                    fprintf(responseFile, "\n\n<html><head>\r\n<title>404 Not Found</title>\r\n</head><body>\r\n<h1>Not Found</h1>\r\n</body></html>");
                }
                
                break;
            case FORBIDDEN:
                
                if(!strcmp(request,"GET")){
                    
                     fprintf(responseFile, "\n\n<html><head>\r\n<title>403 Forbidden</title>\r\n</head><body>\r\n<h1>Forbidden</h1>\r\n</body></html>");
                }
                
                break;
            default: break;

		}
			
	}
    else if(!strcmp(requestList->cmd, "OPTIONS") || !strcmp(requestList->cmd, "TRACE")
            || !strcmp(requestList->cmd, "POST") || !strcmp(requestList->cmd, "DELETE")){
        
        fprintf(responseFile, "\n\n<html><head>\r\n<title>405 Method Not Allowed</title>\r\n</head><body>\r\n<h1>Method Not Allowed</h1>\r\n</body></html>");
        
    }
    
    else if(responseCode == NOT_IMPLEMENTED){
        
        fprintf(responseFile, "\n\n<html><head>\r\n<title>501 Method Not Implemented</title>\r\n</head><body>\r\n<h1>Method Not Implemented</h1>\r\n</body></html>");
        
    }

}

/* Function server_answerRequest()
* Answera a given a request
*
* Parameters:
*	requestList: request to be answered
* 	request: original request file
* 	response: file to save response
*/
void httpServer_answerRequest(HttpRequestList *requestList, FILE *request, FILE* response)
{	
	int code;
    
    if(requestList != NULL){
        if (!strcmp(requestList->cmd, "GET"))
        {
            code = testResource(serverRoot, requestList->params->param);
            buildResponse(requestList, "GET", code, request, response);
        }else if (!strcmp(requestList->cmd, "HEAD"))
        {
            code = testResource(serverRoot, requestList->params->param);
            buildResponse(requestList, "HEAD", code, request, response);
        }else if(!strcmp(requestList->cmd, "OPTIONS") || !strcmp(requestList->cmd, "TRACE")
             || !strcmp(requestList->cmd, "POST") || !strcmp(requestList->cmd, "DELETE")){
            code = NOT_ALLOWED;
            buildResponse(requestList, requestList->cmd, code, request, response);
        
        }else{
            code = NOT_IMPLEMENTED;
            buildResponse(requestList, requestList->cmd, code, request, response);
        }
    }else{
        code = BAD_REQUEST;
        buildResponse(NULL, "", code, request, response);
    }

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
	
    printf("%s",resourcePath);
    
	int file,dir, i;
    
    //Adquire os dados do arquivo
    struct stat fileStat;
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
                return OK;
            }else if (welcomeStat.st_mode & S_IRUSR){
                return OK;
            }
        }

    }
    
    return -1;

}

/* Function addToLog()
* Adds request/response to file log
*
* Parameters:
*	log : log file
*	request: request file
*	response: response file
*/

void httpServer_addToLog(FILE* log, FILE* request, FILE* response)
{
	fseek(log, 0L, SEEK_END);

	fprintf(log, "--- REQUEST ---\r\n\r\n");
    
    char buf[201];
    fseek(request, 0L, SEEK_SET);
    
    while (!feof(request)) {
        size_t res = fread(buf, 1, (sizeof buf)-1, request);
        buf[res] = 0;
        fprintf(log, "%s", buf);
    }

    fprintf(log, "\r\n--- RESPONSE ---\r\n\r\n");
    fseek(response, 0L, SEEK_SET);
    
    while (!feof(response)) {
        size_t res = fread(buf, 1, (sizeof buf)-1, response);
        int i;
        for(i = 0; i < res - 1; i++){
            if(buf[i] == '\n' && buf[i + 1] == '\n'){
                buf[i + 1] = 0; break;
            }
        }
        if(buf[i+1] == 0 && i != res-1){
            fprintf(log, "%s", buf);
            break;
        }else{
            buf[res] = 0;
            fprintf(log, "%s", buf);
        }

    }
	
	fprintf(log,"\n********************************************************************\r\n");
}

int main(int argc, char **argv)
{ 
	HttpRequestList *requestList;
	FILE *request, *response, *log; 

	// prepare webspace 
	httpServer_setServerRoot(argv[1]); 

	yyin = fopen(argv[2], "r"); 
	request = yyin; 
	
	response = fopen(argv[3], "w+"); 

	log = fopen(argv[4], "a"); 

	// call parser 
	if(!yyparse()) 
	{ 
		requestList = httpParser_getRequestList(); 
		httpServer_answerRequest(requestList, request, response); 	
		//httpParser_printRequestList(requestList);
    }
    else{ // ERROR Bad Request
        httpServer_answerRequest(NULL, request, response);
    }
	
		
	httpServer_addToLog(log, request, response); 
}
