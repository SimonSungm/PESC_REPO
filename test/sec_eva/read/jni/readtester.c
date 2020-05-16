#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define TESTNUM 5000

int main()
{
    int i;
    int fd;
    char buf[1000];

    fd = open("testdata",O_RDONLY);
    printf("fd: %d\n",fd);
    for(i = 0; i < TESTNUM;i ++)
    {
        //printf("%ld",read(fd, buf, 100));
        if(read(fd, buf, 100) != 100) printf("Error in read!\n");
        if(read(fd, buf, 100) != 100) printf("Error in read!\n");
        usleep(100000);
    }
    close(fd);
    printf("finish\n");
    printf("PID:%d\n",getpid());

    return 0;
}
