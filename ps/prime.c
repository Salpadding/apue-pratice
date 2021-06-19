#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LEFT  30000000
#define RIGHT 30000200

int main() {
    int i;
    pid_t pid;
    
    for(i = LEFT; i < RIGHT + 1; i++) {
        pid = fork();

        if(pid < 0) {
            perror("fork()");
            exit(0);
        }

        // child process
        if(pid == 0) {
            int mark = 1;
            for(int j = 2; j < i / 2; j++) {
                if(i % j == 0) {
                    mark = 0;
                    break;
                }
            }

            if(mark) {
                printf("find prime %d\n", i);
            }         
            // 这里如果不 exit 会产生死循环   
            exit(0);
        }


    }

    
    for(int j = LEFT; j < RIGHT + 1; j++) {
        wait(NULL);
    }
}