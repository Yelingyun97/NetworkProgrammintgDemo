#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *msg);

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);
		exit(1);
	}

	int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
	
	struct sockaddr_in mul_adr;
	int time_live = TTL;
	FILE *fp;

	memset(&mul_adr, 0, sizeof(mul_adr));
	mul_adr.sin_family = AF_INET;
	mul_adr.sin_addr.s_addr = inet_addr(argv[1]);
	mul_adr.sin_port = htons(atoi(argv[2]));

	setsockopt(send_sock, IPPROTO_IP,
			IP_MULTICAST_TTL, &time_live, sizeof(time_live));
	
	if ((fp = fopen("news.txt", "r")) == NULL)
		error_handling("fopen() error");

	char buf[BUF_SIZE];
	while (fgets(buf, BUF_SIZE, fp) != NULL)
	{
		sendto(send_sock, buf, strlen(buf), 
				0, (struct sockaddr*)&mul_adr, sizeof(mul_adr));
		sleep(2);
	}
	fclose(fp);
	close(send_sock);
	return 0;
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
