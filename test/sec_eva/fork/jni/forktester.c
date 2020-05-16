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
        fpid = fork();
        if(fpid < 0) printf("Error in fork()");
        else if(fpid == 0) return 0;


        fpid = fork();
        if(fpid < 0) printf("Error in fork()");
        else if(fpid == 0) return 0;

        usleep(100000);
    }

    printf("finish\n");
    printf("PID:%d\n",getpid());

    return 0;
}
