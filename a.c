#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
void error_handling(char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("proc: %d removed proc: %d\n", getpid(), pid);
}

void timeout(int sig)
{
	puts("time out");
}

int main()
{
	struct sigaction act;
	//act.sa_handler = read_childproc;
	//sigemptyset(&act.sa_mask);
	//act.sa_flags;
	//sigaction(SIGCHLD, &act, 0);

	act.sa_handler = timeout;
	sigemptyset(&act.sa_mask);
	act.sa_flags;
	sigaction(SIGALRM, &act, 0);
	alarm(2);
	
	pid_t pid = fork();
	if (pid == 0)
	{
		pid = fork();
		if (pid == 0)
		{
			sleep(10);
			printf("grandchild proc id: %d\n", getpid());
			return 0;
		}
		else
		{
			sleep(10);
			printf("child process id: %d\n", getpid());
		}
		return 0;
	}
	else
	{
		sleep(10);
		printf("parent process id: %d\n", getpid());
	}
	return 0;
}
