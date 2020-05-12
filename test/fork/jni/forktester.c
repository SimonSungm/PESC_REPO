#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <syscall.h>
#include <signal.h>
#include <time.h>

#define TESTNUM1 10
#define TESTNUM2 1000

int main()
{
    unsigned long long begin_time, end_time, duration;
    int i, j;
    unsigned long long sum = 0;
    int fpid;

    for(i = 0; i < TESTNUM1; i++)
    {
        begin_time = (unsigned long long)clock();

        for(j = 0; j < TESTNUM2; j++){
            //syscall(SYS_getpid);        //getpid();
            //if(syscall(SYS_read, fd, buf, 100) != 100) printf("Error in read!\n");
            fpid = fork();
            if(fpid < 0){
                printf("Error in fork()");
                exit(0);
            }
            else if(fpid == 0) exit(0);
        }
        end_time = (unsigned long long)clock();

        duration = end_time - begin_time;

        printf("Testnum: %u, duration: %llu\n", TESTNUM2, duration);
        if(i >= 1 && i < TESTNUM1-1)
        {
            sum+=duration;
        }
        //printf("testnum: %u, begin: %llu, end: %llu, duration: %llu\n", TESTNUM, begin_time, end_time, end_time - begin_time);
    }
    printf("avg:%llu\n", sum / (TESTNUM1-2));

    return 0;
}
