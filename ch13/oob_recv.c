#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define BUF_SIZE 30
void error_handling(char *msg);
void urg_handler(int signo);

int serv_sock, conn_sock;

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1)
		error_handling("socket() error");

	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t saddr_sz, caddr_sz;

	saddr_sz = sizeof(serv_addr);
	memset(&serv_addr, 0, saddr_sz);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, saddr_sz) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	struct sigaction act;
	act.sa_handler = urg_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	caddr_sz = sizeof(clnt_addr);
	conn_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &caddr_sz);
	if (conn_sock == -1)
		error_handling("accept() error");
	
	fcntl(conn_sock, F_SETOWN, getpid());
	sigaction(SIGURG, &act, 0);

	int str_len;
	char buf[BUF_SIZE];
	while ((str_len = recv(conn_sock, buf, sizeof(buf), 0)) != 0)
	{
		if (str_len == -1)
			continue;
		buf[str_len] = 0;
		puts(buf);
	}
	close(conn_sock);
	close(serv_sock);
	return 0;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void urg_handler(int signo)
{
	char buf[BUF_SIZE];
	int str_len = recv(conn_sock, buf, sizeof(buf) - 1, MSG_OOB);
	buf[str_len] = 0;
	printf("Urgent meesage: %s\n", buf);
}
