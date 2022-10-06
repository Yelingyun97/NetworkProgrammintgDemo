#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define BUF_SIZE 30
void error_handling(char *msg);

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}

	int serv_sock, conn_sock;
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

	caddr_sz = sizeof(clnt_addr);
	conn_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &caddr_sz);
	if (conn_sock == -1)
		error_handling("accept() error");
	
	char buf[BUF_SIZE];
	int str_len = recv(conn_sock, buf, sizeof(buf) - 1, MSG_PEEK);
	buf[str_len] = 0;
	printf("Buffering %d bytes: %s\n", str_len, buf);

	str_len = recv(conn_sock, buf, sizeof(buf) - 1, 0);
	buf[str_len] = 0;
	printf("Read again: %s\n", buf);

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
