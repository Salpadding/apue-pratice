#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid;

    printf("[%d]: Begin!\n", getpid());
    // fork 之前应该刷新缓冲区, 防止缓冲区被复制
    fflush(NULL);
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
    return 0;
}