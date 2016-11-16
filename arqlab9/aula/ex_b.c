#include <stdio.h>
#include <signal.h>

int test_variavel;

void handler (int signal_number) {

	printf("Alguem Saiu!");
	test_variavel--;

}

main()
{

struct sigaction act;
    memset (&act, 0, sizeof (act));
    act.sa_handler= handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD,&act,NULL) == -1){

        printf("Handle Error");

    }


int pid;
	printf ("pid(processo)=%d, pid(processo-pai)=%d\n",getpid(),getppid());
	test_variavel = 10;
	pid = fork();
	if (pid == 0)
	{
		test_variavel++;
		printf("\nProcesso-filho:%d\n",test_variavel);
		printf("pid(processo-filho)=%d, pid(processo-pai)=%d\n",getpid(),getppid());
		exit(0);
	}
	else
	{
		printf("\nProcesso-pai:%d\n",test_variavel);
		printf("pid(processo-filho)=%d\n",pid);
		sleep(10);
	}
	printf("Processo %d terminou!%d\n",getpid(),test_variavel);
}
