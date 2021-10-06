// mmap 实现共享内存
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/select.h>

#define BUF_SIZE 4096

void my_sleep(int seconds)
{
    struct timeval tm;
    tm.tv_sec = seconds;
    tm.tv_usec = 0;
    select(0, NULL, NULL, NULL, &tm);
}

int main(int argc, char** argv)
{
    char* mem = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED , -1, 0);

    if(mem == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    pid_t pid = fork();

    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if (pid == 0)
    {
        // parent, read from memory

        while(1)
        {
            printf("%s\n", mem);
            my_sleep(1);
        }
    } else {
        // child write to memory
        int count = 0;
        while(1)
        {
            snprintf(mem, BUF_SIZE, "hello world, count = %d", count++);
            my_sleep(1);
        }
    }

    exit(1);
}