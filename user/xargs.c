#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"
#define MAX_LEN 100
   
int main(int argc, char *argv[]) {
	char *command = argv[1];
	char bf;
	char paramv[MAXARG][MAX_LEN]; // 参数
	char *m[MAXARG];
   
	while (1) { //有多行就要执行多次
		int count = argc-1;  // #当前参数数量
		memset(paramv, 0, MAXARG * MAX_LEN);
		
        //复制相应参数
		for (int i=1; i<argc; i++) {
			strcpy(paramv[i-1], argv[i]);
		}
   
		int p = 0; 
		int flag = 0; // flag用于判断是否需要吞掉当前的空格
		int read_result;
   
		while (((read_result = read(0, &bf, 1))) > 0 && bf != '\n') {
			if (bf == ' ' && flag == 1) {
				count++;
				//重置
				p = 0;
				flag = 0;
			}
			else if (bf != ' ') {
				paramv[count][p++] = bf;
				flag = 1;
			}
		}
		// 如果已经读完了就全部结束
		if (read_result <= 0) {
			break;
		}
        //复制参数给底下的exec用
		for (int i=0; i<MAXARG-1; i++) {
			m[i] = paramv[i];
		}
        //尾零结束
		m[MAXARG-1] = 0;
		if (fork() == 0) {
			exec(command, m);
			exit(0);
		} else {
			wait((int *) 0);
		}
	}
	exit(0);
}