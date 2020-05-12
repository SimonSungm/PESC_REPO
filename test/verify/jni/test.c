#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syscall.h>

#define TEST_NUM 10
#define MAX_LEN 100
#define NR_getcanary 295	//请确保系统调用号正确

int main()
{
	char command[MAX_LEN];
	for(int i = 0; i < TEST_NUM; i ++){
		printf("canary:%lx\n", syscall(NR_getcanary));
	}
	return 0;
}
