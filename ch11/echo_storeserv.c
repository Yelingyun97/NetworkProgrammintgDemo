#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 1024

void read_childproc(int sig);
void error_handling(char *message);

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		printf("Usage:%s <port>\n", argv[0]);
		exit(1);
	}

	int serv_sock, clnt_sock;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		error_handling("socket() error");

	struct sockaddr_in serv_addr;
	socklen_t saddr_len = sizeof(serv_addr);

	memset(&serv_addr, 0, saddr_len);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, saddr_len) == -1)
		error_handling("bind() error");

	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	struct sigaction act;

	act.sa_handler = read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGCHLD, &act, 0);

	struct sockaddr_in clnt_addr;
	socklen_t caddr_len = sizeof(clnt_addr);
	char msg[BUF_SIZE];

	int fds[2];
	pipe(fds);
	pid_t pid = fork();
	if (pid == 0)
	{
		char msgbuf[BUF_SIZE];
		FILE *fp = fopen("echomsg.txt", "wt");

		for (int i = 0; i < 10; i++)
		{
			int len = read(fds[0], msgbuf, BUF_SIZE);
			fwrite((void*)msgbuf, 1, len, fp);
		}
		fclose(fp);
		return 0;
	}

	while (1)
	{
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &caddr_len);
		if (clnt_sock == -1)
			continue;
		else
			puts("new client connected...");

		pid_t pid = fork();
		if (pid == -1)
		{
			close(clnt_sock);
			continue;
		}

		if (pid == 0)
		{
			close(serv_sock);

			int str_len;
			while ((str_len = read(clnt_sock, msg, BUF_SIZE)) != 0)
			{
				write(clnt_sock, msg, str_len);
				write(fds[1], msg, str_len);
			}

			close(clnt_sock);
			puts("client disconnected...");
			return 0;
		}
		else
			close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void read_childproc(int sig)
{
	int status;
	if (sig == SIGCHLD)
	{
		pid_t pid =  waitpid(-1, &status, WNOHANG);

		if (WIFEXITED(status))
		{
			printf("Removed proc id: %d\n", pid);
			printf("Child send: %d\n", WEXITSTATUS(status));
		}
	}
}	

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
