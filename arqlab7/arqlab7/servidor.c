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


//#include "parser.h"
//#include "parser.tab.h"

#include "custom_parser.h"
#include "custom_parser.tab.h"

extern FILE* yyin;

static char serverRoot[256];	

#define PATH_OFFSET 20

#define OK	200
#define NOT_FOUND 404
#define FORBIDDEN 403


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
	}

    time_t rawtime;
    char buf[80];
    
    time( &rawtime );
    
    strftime(buf,80,"%a, %d %b %Y %H:%M:%S %Z", localtime( &rawtime ));
	fprintf(responseFile, "Date: %s\r\n", buf);

	fprintf(responseFile, "Server: Servidor HTTP ver 0.1 de Joao Marcos \r\n");

	//print connection type using requestList, this is only an example
	fprintf(responseFile, "Connection: %s\r\n", httpParser_getParams(CONNECTION)->param);
	
	if (strcmp(request,"GET")==0 || strcmp(request,"HEAD")==0 )
	{
		fprintf(responseFile, "Content-Type: text/html\r\n");
        
        struct stat statbuff;
        stat(resourcePath, &statbuff);
        
		switch (responseCode)
		{				
			case OK:
                
                strftime(buf, 80, "%a, %d %b %Y %H:%M:%S %Z", localtime(&(statbuff.st_ctime)));

				fprintf(responseFile, "Last-Modified: %s\r\n", buf);
				break;
            case NOT_FOUND:
                fprintf(responseFile, "File not found");
                break;
            case FORBIDDEN:
                fprintf(responseFile, "Access Forbidden");
                break;
            default: break;
			//case NOT_FOUND,FORBIDDEN, etc
		}
			
		fprintf(responseFile, "Content-Length: %d\r\n\r\n", (int)statbuff.st_size);
		
        if(!strcmp(request,"GET")){
            int fid;
            fid = open(resourcePath, O_RDONLY);
            read(fid, resourcePath, statbuff.st_size);
            fwrite(resourcePath, sizeof(char), statbuff.st_size, responseFile);
            close(fid);
        }
	}
	//else -> HEAD, ETC

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

	if (!strcmp(requestList->cmd, "GET"))
	{
		code = testResource(serverRoot, requestList->params->param);
		buildResponse(requestList, "GET", code, request, response);
    }else if (!strcmp(requestList->cmd, "HEAD"))
    {
        code = testResource(serverRoot, requestList->params->param);
        buildResponse(requestList, "HEAD", code, request, response);
    }
	//else if ("HEAD", "OPTIONS", etc)	
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

	/*if (stat(resourcePath, &statbuf) == -1) { 

	} 
	else { 
		switch (statbuf.st_mode & S_IFMT) { 			
			case S_IFREG: 
				returnValue = OK;
				break; 			
		}
	}*/
	
	int file,dir, i;
    
    //Adquiri os dados do arquivo
    struct stat fileStat;
    if(stat(resourcePath,&fileStat) < 0){
        //printf("ERRO %d: Not Found\n", NOT_FOUND);
        return NOT_FOUND;
    }
    
    //Verifica permissao de leitura
    if (!(fileStat.st_mode & S_IRUSR)){
        //printf("ERRO %d: FORBIDDEN\n", FORBIDDEN);
        return FORBIDDEN;
    }
    
    if(!(S_ISDIR(fileStat.st_mode))){
        //ARQUIVO
        //read_and_print_file(resourcePath, fileStat);
        return OK;
    }else{
        //DIRETORIO
        //Verifica permissao de varredura
        if (!((fileStat.st_mode & S_IXUSR))){
            //printf("ERRO %d: FORBIDDEN\n", FORBIDDEN);
            return FORBIDDEN;
        }
        else{
            char *strIndex = malloc(sizeof(char)*(strlen(resourcePath)+12));
            char *strWelcome = malloc(sizeof(char)*(strlen(resourcePath)+14));;
            struct stat indexStat, welcomeStat;
            
            strcpy(strIndex,resourcePath); strcat(strIndex,"/index.html");
            strcpy(strWelcome,resourcePath); strcat(strWelcome,"/welcome.html");

            //Verifica existenci a de index.html e welcome.html
            if(stat(strIndex,&indexStat) < 0 && stat(strWelcome,&welcomeStat)){
                //printf("ERRO %d: Not Found\n", NOT_FOUND);
                return NOT_FOUND;
            }
            
            //Verifica a permissao de leitura de index.html e welcome.html
            if (!((indexStat.st_mode & S_IRUSR) && (welcomeStat.st_mode & S_IRUSR))){
                //printf("ERRO %d: FORBIDDEN\n", FORBIDDEN);
                return FORBIDDEN;
            }
            
            if (indexStat.st_mode & S_IRUSR){
                //read_and_print_file(strIndex, indexStat);
                return OK;
            }else if (welcomeStat.st_mode & S_IRUSR){
                //read_and_print_file(strWelcome, welcomeStat);
                return OK;
            }
        }

    }

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
	fprintf(log,"colocar aqui o conteúdo do arquivo 'request'");


	fprintf(log, "\r\n--- RESPONSE ---\r\n\r\n");
	fprintf(log,"colocar aqui o conteúdo do arquivo 'response' (mas só o cabeçalho da mesma, sem o conteúdo do recursos solicitado, no caso de GET)");
	
	fprintf(log,"\n********************************************************************\r\n");
}

main(int argc, char **argv) 
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
		httpParser_printRequestList(requestList);
	} 
	//else ERROR
		
	httpServer_addToLog(log, request, response); 
}
