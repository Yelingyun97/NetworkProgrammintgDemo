#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main()
{
	int fds1[2], fds2[2];
	char str1[] = "Who are you?";
	char str2[] = "Thank you for your message";
	char buf[BUF_SIZE];

	pipe(fds1);
	pipe(fds2);
	pid_t pid = fork();
	if (pid == 0)
	{
		write(fds1[1], str1, sizeof(str1));
		read(fds2[0], buf, BUF_SIZE);
		printf("child proc output: %s\n", buf);
	}
	else
	{
		read(fds1[0], buf, BUF_SIZE);
		printf("parent proc output: %s\n", buf);
		write(fds2[1], str2, sizeof(str2));
		sleep(3);
	}
	return 0;
}

