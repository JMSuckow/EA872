#include <stdio.h>
#include <sys/wait.h>
#include <sys/signal.h>

main(){
int pid1, pid2,i;
long j;
void ger_sinal();
       signal(SIGCHLD,ger_sinal);
       pid1 = fork();
       if (pid1 == 0){
               while (1){
		   printf("Processo %d esta' ativo/em execucao\n",getpid());
		   sleep(1);
		   printf("*** 1 ***\n");
		   fflush(0);
		   for(j=0;j<1000000;j++);
               }
       }
       pid2 = fork();
       if (pid2 == 0){
               while (1){
		   printf("Processo %d esta' ativo/em execuca \n",getpid());
		   sleep(1);
		   printf("*** 2 ***\n");
		   fflush(0);
		   for(j=0;j<1000000;j++);
               }
       }
       sleep(5);
       printf("%d:vou parar o 1o.\n",getpid());
       fflush(0);
       kill (pid1,SIGSTOP);
       sleep(5);
       printf("%d:vou parar o 2o.\n",getpid());
       fflush(0);
       kill (pid2,SIGSTOP);
       sleep(5);
       printf("%d:vou continuar o 1o.\n",getpid());
       fflush(0);
       kill (pid1,SIGCONT);
       sleep(5);
       printf("%d:vou continuar o 2o.\n",getpid());
       fflush(0);
       kill (pid2,SIGCONT);
       sleep(5);
       printf("%d:vou matar o 1o.\n",getpid());
       fflush(0);
       kill (pid1,SIGINT);
       sleep(5);
       printf("\n%d:vou matar o 2o.\n",getpid());
       fflush(0);
       kill (pid2,SIGINT);

      /* Sinais são assíncronos,  logo, não se sabe quando vão "chegar". Se o programa acabar logo após essas chamadas kill(pidX,SIGINT), os sinais vão chegar tarde demais (o programa já terá finalizado) e não serão capturados!
       */ 
                                                                                                                                                                                    
       for(i = 0; i < 5; i++){
	 printf("%d: esperando sinal \"chegar\"...\n",getpid());
	 fflush(0);
	 for(j=0;j<1000000;j++);
	 sleep(1);
       }
}

void ger_sinal(){

  int pid;
  int estado;

 /* Este loop é um detalhe importante. Um sinal SIGCHLD (em sistemas BSD) pode estar "representando" múltiplos filhos e então o sinal SIGCHLD significa que um ou mais filhos tiveram seu estado alterado.
  */

  do{
    pid = wait3(&estado,WNOHANG,NULL);
    printf("%d: Um sinal de mudanca do estado do processo-filho %d foi captado!\n", 
           getpid(),pid);
    if (pid > 0){
       printf("O processo-filho %d foi extinto! Estado=%d\n", pid,estado>>8);
    }
  } while( pid > 0 && printf("em loop!  \n"));

  signal(SIGCHLD,ger_sinal);
}
