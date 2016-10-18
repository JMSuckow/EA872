#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define NOT_FOUND 404
#define FORBIDDEN 403

void read_and_print_file(char* file_string, struct stat fileStat);

int main(int argc, char *argv[]){
    int file,dir, i;
    
    //Verifica número de argumentos
    if(argc != 3){
        
        printf("ERRO: Número de parâmetros\n");
        return 1;
        
    }
    
    //Combina o caminho das strings
    char* string =malloc(sizeof(char)*(strlen(argv[1])+strlen(argv[2])+2));
    strcpy(string,argv[1]); strcat(string,"/"); strcat(string,argv[2]);
    
    //Adquiri os dados do arquivo
    struct stat fileStat;
    if(stat(string,&fileStat) < 0){
        printf("ERRO %d: Not Found\n", NOT_FOUND);
        return NOT_FOUND;
    }
    
    //Verifica permissao de leitura
    if (!(fileStat.st_mode & S_IRUSR)){
        printf("ERRO %d: FORBIDDEN\n", FORBIDDEN);
        return FORBIDDEN;
    }
    
    if(!(S_ISDIR(fileStat.st_mode))){
        //ARQUIVO
        read_and_print_file(string, fileStat);
        return 0;
    }else{
        //DIRETORIO
        //Verifica permissao de varredura
        if (!((fileStat.st_mode & S_IXUSR))){
            printf("ERRO %d: FORBIDDEN\n", FORBIDDEN);
            return FORBIDDEN;
        }
        else{
            char *strIndex = malloc(sizeof(char)*(strlen(string)+12));
            char *strWelcome = malloc(sizeof(char)*(strlen(string)+14));;
            struct stat indexStat, welcomeStat;
            
            strcpy(strIndex,string); strcat(strIndex,"/index.html");
            strcpy(strWelcome,string); strcat(strWelcome,"/welcome.html");

            //Verifica existenci a de index.html e welcome.html
            if(stat(strIndex,&indexStat) < 0 && stat(strWelcome,&welcomeStat)){
                printf("ERRO %d: Not Found\n", NOT_FOUND);
                return NOT_FOUND;
            }
            
            //Verifica a permissao de leitura de index.html e welcome.html
            if (!(indexStat.st_mode & S_IRUSR) && !(welcomeStat.st_mode & S_IRUSR)){
                printf("ERRO %d: FORBIDDEN\n", FORBIDDEN);
                return FORBIDDEN;
            }
            
            if (indexStat.st_mode & S_IRUSR){
                read_and_print_file(strIndex, indexStat);
                return 0;
            }else if (welcomeStat.st_mode & S_IRUSR){
                read_and_print_file(strWelcome, welcomeStat);
                return 0;
            }
        }

    }

}

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