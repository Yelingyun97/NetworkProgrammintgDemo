#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define BUF_SIZE 100
#define MAX_CLNT 256

void *handle_clnt(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int clnt_cnt = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutex;
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <PORT>\n", argv[0]);
		exit(1);
	}

	pthread_t tid;
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	socklen_t saddr_sz, caddr_sz;

	pthread_mutex_init(&mutex, NULL);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);

	saddr_sz = sizeof(serv_addr);
	memset(&serv_addr, 0, saddr_sz);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if (bind(serv_sock, (struct sockaddr*)&serv_addr, saddr_sz) == -1)
		error_handling("bind() error");
	if (listen(serv_sock, 5) == -1)
		error_handling("listen() error");

	while (1)
	{
		caddr_sz = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &caddr_sz);
		
		pthread_mutex_lock(&mutex);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutex);

		pthread_create(&tid, NULL, handle_clnt, &clnt_sock);
		pthread_detach(tid);
		printf("Connected client IP: %s\n", inet_ntoa(clnt_addr.sin_addr));
	}
	close(serv_sock);
	pthread_mutex_destroy(&mutex);
	return 0;
}

void *handle_clnt(void *arg)
{
	int clnt_sock = *((int*)arg);
	char buf[BUF_SIZE];
	int str_len;

	while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
		send_msg(buf, str_len);

	pthread_mutex_lock(&mutex);
	for (int i = 0; i < clnt_cnt; i++)
	{
		if (clnt_sock == clnt_socks[i])
		{
			while (i++ < clnt_cnt -1)
				clnt_socks[i - 1] = clnt_socks[i];
			break;
		}
	}
	clnt_cnt--;
	pthread_mutex_unlock(&mutex);
	close(clnt_sock);
	return NULL;
}

void send_msg(char *msg, int len)
{
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < clnt_cnt; i++)
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutex);
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
