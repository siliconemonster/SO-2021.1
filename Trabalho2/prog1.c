#include <stdio.h>
#include <wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
	int status, j;
	int pipePtC[2], pipeCtP[2];
	pipe(pipePtC);
	pipe(pipeCtP);
	// Insira um comando para pegar o PID do processo corrente e mostre na tela da console.
	int pid = getpid();
	printf("PID do processo atual: %d\n", pid);

	// insira um comando para criar um subprocesso
	int ret = fork();

	if (ret != 0)
	{
		close(pipePtC[0]);
		close(pipeCtP[1]);
		// Faça com que o processo pai execute este trecho de código
		// Mostre na console o PID do processo pai e do processo filho
		printf("(pai) PID do pai: %d\n", pid);
		printf("(pai) PID do filho: %d\n", ret);
		// Monte uma mensagem e a envie para o processo filho

		const char messageFromParent[] = "Mensagem do pai pro filho";
		write(pipePtC[1], messageFromParent, sizeof(messageFromParent));
		close(pipePtC[1]);
		// Mostre na tela o texto da mensagem enviada
		printf("(pai) Mensagem: '%s'\n", messageFromParent);
		// Aguarde a resposta do processo filho
		// Mostre na tela o texto recebido do processo filho
		// Aguarde mensagem do filho e mostre o texto recebido
		char buf;
		while (read(pipeCtP[0], &buf, 1) > 0)
		{
			printf("%c", buf);
		}
		printf("\n");
		close(pipeCtP[0]);
		// Aguarde o término do processo filho
		wait(&status);
		// Informe na tela que o filho terminou e que o processo pai também vai encerrar
		if (WIFEXITED(status))
		{
			printf("(pai) Processo filho encerrou normalmente; processo pai está encerrando.\n");
		}
		else
		{
			printf("(pai) Processo filho encerrou anormalmente; processo pai está encerrando.\n");
		}
	}
	else
	{
		// Faça com que o processo filho execute este trecho de código
		// Mostre na tela o PID do processo corrente e do processo pai
		int childPid = getpid();
		int parentPid = getppid();
		printf("(filho) PID do processo corrente: %d\n", childPid);
		printf("(filho) PID do processo pai: %d\n", parentPid);
		// Aguarde a mensagem do processo pai e ao receber mostre o texto na tela
		close(pipePtC[1]);
		close(pipeCtP[0]);

		char buf;
		while (read(pipePtC[0], &buf, 1) > 0)
		{
			printf("%c", buf);
		}
		printf("\n");
		close(pipePtC[0]);
		// Envie uma mensagem resposta ao pai
		const char messageFromChild[] = "Resposta do filho pro pai";
		write(pipeCtP[1], messageFromChild, sizeof(messageFromChild));
		// Execute o comando “for” abaixo
		for (j = 0; j <= 10000; j++)
			;
		// Envie mensagem ao processo pai com o valor final de “j”
		char bufJ[11];
		snprintf(bufJ, 11, "\nj = %d", j);
		write(pipeCtP[1], bufJ, sizeof(bufJ));
		close(pipeCtP[1]);
		// Execute o comando abaixo e responda às perguntas
		execl("/Bin/ls", "ls", NULL);
		printf("(filho) Processo filho está encerrando.\n");
		// O que acontece após este comando?
		// O que pode acontecer se o comando “execl” falhar?
	}
	exit(0);
}