#include "kernel/types.h"
#include "user/user.h"
#define READ 0
#define WRITE 1

//递归函数，用于寻找所有的prime
void prime(int pl[])
{
    int pr[2];//右管道
    int data;//读到的数据

    close(pl[WRITE]);
    int result = read(pl[READ],&data,4);
    if(result == 0)//已经结束了
    {
        exit(0);
    }
    pipe(pr);

    int flag=fork();
    if(flag>0)//父进程
    {
        close(pr[READ]);
        int prime = data;
        printf("prime %d\n",prime);
        while(read(pl[READ],&data,4))
        {
            if(data%prime!=0)
            {
                write(pr[WRITE],&data,4);
            }
        }
        close(pr[WRITE]);
        wait((int*)0);
        exit(0);
    }
    else if(flag==0)//子进程
    {
        prime(pr);
    }
    else
    {
        exit(1); //error
    }

}


int main(int argc,char* argv[] )
{
	int pl[2];//左管道
    pipe(pl);
    int flag = fork();
    if(flag>0) //父进程的返回
    {
        close(pl[READ]);
        for(int i =2;i<=35;i++)
        {
            write(pl[WRITE],&i,4); //4是int的大小
        }
        close(pl[WRITE]);
        wait((int*)0);//等待子进程结束
    }
    else if(flag==0)//子进程的返回
    {
        prime(pl);
    }
    else//fork异常了
    {
        exit(1);
    }
    exit(0);
}
