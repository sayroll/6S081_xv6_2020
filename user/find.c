#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

void find(char* path ,char* target)
{
    //参考ls.c的变量以及读数据
	int fd;
	struct stat st;//状态
	struct dirent de;//目录项
	char buf[512], *p;//buf存路径，p是添加最新路径名的指针

    if ((fd = open(path, 0)) < 0) {
		fprintf(2, "ERROR: cannot open %s\n", path);
		return;
	}
   	
	if (fstat(fd, &st) < 0) {
		fprintf(2, "ERROR: cannot stat %s\n", path);
		close(fd);
		return;
	}

    while(read(fd,&de,sizeof(de)) == sizeof(de))
    {
        strcpy(buf,path);
        p=buf+strlen(buf);
        *p++='/';//分开
        if(de.inum==0)//没有项
            continue;
        
        memmove(p,de.name,DIRSIZ);//复制名字到buf里
        p[DIRSIZ]='\0';//尾零代表终结

        stat(buf,&st);//看是什么文件

        if(st.type==T_FILE)
        {
            if(strcmp(de.name,target) == 0)
                printf("%s\n",buf);
        }
        else if(st.type == T_DIR)
        {
            if(strcmp(de.name,".")!=0 && strcmp(de.name,"..")!=0)
            {
                find(buf,target);
            }
        }

    }
    close(fd);
    return;
}

int main(int argc,char* argv[])
{
    if(argc!=3)//参数个数有问题，要求只有两个参数
    {
        printf("Error: please input only two parameters\n");
        exit(1);
    }
    char *path = argv[1];
    char *target = argv[2];
    find(path,target);
    exit(0);
}