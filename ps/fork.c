#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid;

    printf("[%d]: Begin!\n", getpid());

    pid = fork();

    if(pid < 0) {
        perror("fork()");
        exit(0);
    }

    if(pid == 0) {
        printf("[%d]: Child is working!\n", getpid());
    } else {
        printf("[%d]: Parent is working\n", getpid());
    }

    printf("[%d]: End\n", getpid());

    sleep(1000);
    return 0;
}