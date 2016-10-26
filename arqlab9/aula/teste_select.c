/* Programa teste_select.c */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  int n, c;
  long int tolerancia = 5;
  fd_set fds;
  struct timeval timeout;
  int fd;
  
  fd = 0;
  FD_ZERO(&fds);
  FD_SET(fd, &fds);
  timeout.tv_sec = tolerancia;
  timeout.tv_usec = 0;

  n = select(1, &fds, (fd_set *)0, (fd_set *)0, &timeout);

  if(n > 0 && FD_ISSET(fd, &fds))
    {
      c = getchar();
      printf("Caractere %c teclado.\n", c);
    }
  else if(n == 0)
        {
        printf("Nada foi teclado em %ld s.\n", tolerancia);
        }
       else perror("Erro em select():");
  exit(1);
}
