#include "func.h"

int shmid;
int pidA;	// 存放对端进程id
char *p;	// 共享内存指针

// 回收共享内存资源前先杀死对端进程，否则回收失败
void handle(int num)
{
	kill(pidA, SIGINT);
	shmdt(p);
	int ret;
	if(-1 == (ret=shmctl(shmid, IPC_RMID, NULL))) 
	{
		perror("shmctl");
		return (void)-1;
	}
	exit(0);
}

int main(int argc, char **argv)
{
	signal(SIGINT, handle);
	if(-1 == (shmid=shmget(1235, 4096, IPC_CREAT|0666)))
	{
		perror("shmget");
		return -1;
	}
	
	if((char*)-1 == (p=(char*)shmat(shmid, NULL, 0)))
	{
		perror("shmat");
		return -1;
	}
	
	// 管道文件为单工通信方式
	// B进程通过管道文件fifo1的写端fdw向A进程发送数据
	// B进程通过管道文件fifo2的读端fdr接收B进程的数据
	int fdr, fdw;
	if(-1 == (fdw=open("fifo1", O_WRONLY)) || -1 == (fdr=open("fifo2", O_RDONLY)))
	{
		perror("open fifo1 or open fifo2");
		return -1;
	}
	
	// 通信之前先通过管道互相告知对方自己的进程id
	char s1[10] = {0};
	char s2[10] = {0};
	sprintf(s1, "%d\n", getpid());
	write(fdw, s1, strlen(s1) - 1);
	read(fdr, s2, strlen(s1) - 1);
	pidA = atoi(s2);
	printf("pipe connect success, B to B1 shmid:[%d], pidA:[%d], pidB:[%d]\n", shmid, pidA, getpid());
	
	char buf[1024] = {0};
	int ret;
	fd_set rdset;
	while(true)
	{
		FD_ZERO(&rdset);
		FD_SET(0, &rdset);
		FD_SET(fdr, &rdset);
		if((ret=select(fdr+1, &rdset, NULL, NULL, NULL) > 0))
		{
			// fdr可读,则接收数据之后通过共享内存传给B1
			if(FD_ISSET(fdr, &rdset))
			{
				bzero(buf, sizeof(buf));
				if(read(fdr, buf, sizeof(buf)) > 0)
				{
					strncpy(p, buf, sizeof(buf));
				}
				else
				{
					break;
				}
			}
			
			// 标注输入可读,读出来传递给A进程
			if(FD_ISSET(0, &rdset))
			{
				bzero(buf, sizeof(buf));
				if(read(STDIN_FILENO, buf, sizeof(buf)) > 0)
				{
					write(fdw, buf, strlen(buf) - 1);
				}
				else
				{
					break;
				}
			}
		}
	}
	
	close(fdr);
	close(fdw);
	return 0;
}