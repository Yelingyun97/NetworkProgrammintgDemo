#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 30
void error_handling(char *msg);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	int sock;
	struct sockaddr_in serv_addr;
	socklen_t addr_sz;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		error_handling("socket() error");

	addr_sz = sizeof(serv_addr);
	memset(&serv_addr, 0, addr_sz);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	
	if (connect(sock, (struct sockaddr*)&serv_addr, addr_sz) == -1)
		error_handling("connect() error");

	write(sock, "123", strlen("123"));
	close(sock);
	return 0;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
