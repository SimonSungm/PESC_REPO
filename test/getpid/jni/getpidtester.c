#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <syscall.h>
#include <signal.h>
#include <time.h>

#define TESTNUM1 1000
#define TESTNUM2 1000

int main()
{
    unsigned long long begin_time, end_time, duration;
    int i, j;
    unsigned long long sum = 0;

    for(i = 0; i < TESTNUM1; i++)
    {
        begin_time = (unsigned long long)clock();

        for(j = 0; j < TESTNUM2; j++)
            syscall(SYS_getpid);        //getpid();
        
        end_time = (unsigned long long)clock();

        duration = end_time - begin_time;

        printf("Testnum: %u, duration: %llu\n", TESTNUM2, duration);
        if(i >= 100 && i < TESTNUM1-100)
        {
            sum+=duration;
        }
        //printf("testnum: %u, begin: %llu, end: %llu, duration: %llu\n", TESTNUM, begin_time, end_time, end_time - begin_time);
    }
    printf("avg:%llu\n", sum / (TESTNUM1-200));

    return 0;
}
