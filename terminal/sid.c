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
        printf("[%d] child: sid = %d\n", getpid(), getsid(0));

        // move child process to new session
        setsid();
        printf("[%d] child: sid = %d\n", getpid(), getsid(0));
        exit(0);
    } else
    {
        printf("[%d] parent: sid = %d\n", getpid(), getsid(0));
        int status = 0;
        pid_t p = wait(&status);
        // wait for child process success even if child process moved to new session
        printf("[%d] status = %d, p = %d\n", getpid(), status, p);
        exit(0);
    }
}
