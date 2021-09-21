#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() 
{
    int cnt = 0;
    pid_t p = fork();
    cnt++;
    if(p == 0)
    {
        execl("/usr/bin/echo", "echo", "*", NULL);
        printf("execl: this line shoudn't printed\n");
        exit(0);
    }

    p = fork();
    cnt++;

    if(p == 0)
    {
        execlp("echo", "echo", "*", NULL);
        printf("execlp: this line shoudn't printed\n");
        exit(0);
    }

    p = fork();
    cnt++;

    if(p == 0)
    {
        char* envs[2] = {"X_VAR=helloworld", NULL};
        execle("/usr/bin/env", "env", NULL, envs);
        printf("execle: this line shoudn't printed\n");
        exit(0);
    }


    for(int i = 0; i < cnt; i++)
    {
        wait(NULL);
    }
}