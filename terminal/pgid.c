// print the id of process group
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

int main()
{
    pid_t pid = fork();

    if(pid == 0)
    {
        printf("[%d] child: pgid = %d\n", getpid(), getpgrp());

        // move child process to new process group
        setpgid(0, 0);
        printf("[%d] child: pgid = %d\n", getpid(), getpgrp());
        exit(0);
    } else
    {
        printf("[%d] parent: pgid = %d\n", getpid(), getpgrp());
        int status = 0;
        pid_t p = wait(&status);
        // wait for child process success even if child process moved to new process group
        printf("[%d] status = %d, p = %d\n", getpid(), status, p);
        exit(0);
    }
}