#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUF_SIZE 30
void error_handling(char *msg);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);
		exit(1);
	}

	int recv_sock = socket(PF_INET, SOCK_DGRAM, 0);

	struct sockaddr_in addr;
	struct ip_mreq join_addr;
	
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(atoi(argv[2]));

	if (bind(recv_sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
		error_handling("bind() error");

	join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
	join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

	setsockopt(recv_sock, IPPROTO_IP, 
			IP_ADD_MEMBERSHIP, &join_addr, sizeof(join_addr));

	char buf[BUF_SIZE];
	while(1)
	{
		int str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, 0, 0);
		if (str_len < 0)
			break;
		buf[str_len] = 0;
		fputs(buf, stdout);
	}
	close(recv_sock);
	return 0;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
