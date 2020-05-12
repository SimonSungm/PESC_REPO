#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>

#define TEST_NUM 10
#define MAX_LEN 100

int main()
{
	char command[MAX_LEN];
	for(int i = 0; i < TEST_NUM; i ++){
		printf("canary:%lx\n", syscall(295));
		printf("tid:%d\n",gettid());
	}
	printf("%d",gettid());
	return 0;
}
