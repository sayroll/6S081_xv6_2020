#include "kernel/types.h"
#include "user/user.h"

#define READ 0
#define WRITE 1

int main(int argc,char* argv[])
{
	int p1[2]; // for parent process
	int p2[2]; // for child process
	int pid;
	char buf[1];
   
	pipe(p1);// create according pipe
	pipe(p2);
   	
	pid = fork();
   
	if (pid < 0)
		exit(1);
	else if (pid == 0) {
	// child process
		close(p1[WRITE]);
		close(p2[READ]);
		read(p1[READ], buf, 1);
		printf("%d: received ping\n", getpid());
		write(p2[WRITE], " ", 1);
		close(p1[READ]);
		close(p2[WRITE]);
		exit(0);
	} else {
	// parent process
		close(p1[READ]);
		close(p2[WRITE]);
		write(p1[WRITE], " ", 1);
		read(p2[READ], buf, 1);
		printf("%d: received pong\n", getpid());
		close(p1[WRITE]);
		close(p2[READ]);
		exit(0);
	}
}
