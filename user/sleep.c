#include "kernel/types.h"
#include "user/user.h"

int main(int argc,char* argv[])
{
	if(argc<2)// without enough arg
	{
		printf("Error: Please make enough input\n");
		exit(1);
	}
	sleep(atoi(argv[1]));
	exit(0);
}
