#include "func.h"

int main(void)
{
	char buf[1024] = {0};
	int shmid;
	if(-1 == (shmid=shmget(1235, 4096, IPC_CREAT|0666)))
	{
		perror("shmget");
		return -1;
	}
	
	char *p;
	if((char*)-1 == (p=(char*)shmat(shmid, NULL, 0)))
	{
		perror("shmat");
		return -1;
	}
	
	while(true)
	{
		if(!(strcmp(buf, p)))
		{
			continue;
		}
		else
		{
			// 共享内存有数据可读
			bzero(buf, sizeof(buf));
			strcpy(buf, p);
			printf("I am B1, recv from B:[%s]\n", buf);
		}
	}
	
	if(-1 ==(shmctl(shmid, IPC_RMID, 0)))
	{
		perror("shmctl");
		return -1;
	}
	
	return 0;
}