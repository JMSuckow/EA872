#include <stdio.h>

int test_variavel;

main()
{
int pid;
	printf ("pid(processo)=%d, pid(processo-pai)=%d\n",getpid(),getppid());
	test_variavel = 10;
	pid = fork();
	if (pid == 0)
	{
		test_variavel++;
		printf("\nProcesso-filho:%d\n",test_variavel);
		printf("pid(processo-filho)=%d, pid(processo-pai)=%d\n",getpid(),getppid());
	}
	else
	{
		printf("\nProcesso-pai:%d\n",test_variavel);
		printf("pid(processo-filho)=%d\n",pid);
		sleep(10);
	}
	printf("Processo %d terminou!%d\n",getpid(),test_variavel);
}
