#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024
#define MESSAGE "hello pipe\n"

int main()
{
    // create a pipe
    int pd[2];
    pid_t pid;
    char buf[BUF_SIZE];
    ssize_t len;

    if(pipe(pd) < 0)
    {
        perror("pipe()");
        exit(1);
    }

    pid = fork();

    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }


    if(pid == 0)
    {
        // child process
        // read from pipe & write to stdout
        close(pd[1]);

        while(1)
        {
            len = read(pd[0], buf, BUF_SIZE);
            if(len < 0)
            {
                perror("read()");
                break;
            }
            write(STDOUT_FILENO, buf, len);
        }

        close(pd[0]);
    } else {

        // parent process
        // write hello pipe\n to pipe
        close(pd[0]);

        while(1) 
        {
            write(pd[1], MESSAGE, strlen(MESSAGE));
            sleep(1);
        }

        close(pd[1]);
    }


    exit(0);
}