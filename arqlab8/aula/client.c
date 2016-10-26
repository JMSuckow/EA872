#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    
    //if(argc != 2)
      //  exit(1);
    
    fd_set fdset;
    struct timeval tv;
    
    char *ip;
    ip = strdup(argv[1]);
    int port = atoi(argv[2]);
    
    
    
    int soquete;
    struct sockaddr_in destino;
    char msg_ida[1024], msg_volta[1024];
    
    soquete = socket(AF_INET, SOCK_STREAM, 0);
    destino.sin_family = AF_INET;
    destino.sin_port = htons(port);
    inet_aton(ip, &destino.sin_addr.s_addr);
    
    
    fcntl(soquete, F_SETFL, O_NONBLOCK);
    
    connect(soquete, (struct sockaddr*)&destino, sizeof(destino));
    
    FD_ZERO(&fdset);
    FD_SET(soquete, &fdset);
    tv.tv_sec = 10;             /* 10 segundos timeout */
    tv.tv_usec = 0;
    
    int socket_value = select(soquete + 1, NULL, &fdset, NULL, &tv);
    
    if (socket_value == 1)
    {
        int oldfl = fcntl(soquete, F_GETFL);
        fcntl(soquete, F_SETFL, oldfl & ~O_NONBLOCK);
        sprintf(msg_ida, "GET / HTTP/1.1\r\nHost: 143.106.148.79:%d\r\nConnection: close\r\n\n", 80);
        write (soquete, msg_ida, strlen(msg_ida)+1);
        read (soquete, msg_volta, 1024);
        close(soquete);
        printf("%s", msg_volta);
        
        FILE *f;
        f = fopen("index.html", "w");
        
        fprintf(f, "%s", msg_volta);
        
        fclose(f);
        
    }else if(socket_value == 0){ // TimeOut
        
        printf("TimeOut\n");
    
    }else{ //ERRO
        printf("Erro: %s\n", strerror(errno));
    }
    
}
