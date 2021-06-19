#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("begin!\n");
    fflush(NULL);

    int pid = fork();

    if(pid < 0) {
        perror("fork()");
        exit(0);
    }

    if(pid == 0) {
        printf("child!\n");
        fflush(NULL);
        execl("/usr/bin/date", "date", "+%s", NULL);
        perror("execl()");
    }

    wait(NULL);
    printf("end!\n");
    return 0;
}