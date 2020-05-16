#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define TESTNUM 5000

int main()
{
    int i;
    int fpid;

    for(i = 0; i < TESTNUM;i ++)
    {
        syscall(SYS_getpid);
	syscall(SYS_getpid);	

        usleep(100000);
    }

    printf("finish\n");
    printf("PID:%d\n",getpid());

    return 0;
}
