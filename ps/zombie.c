#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define ZOMBINE 1000

int main() {
    for(int i = 0; i < ZOMBINE; i++) {
        pid_t p = fork();
        if(p < 0) {
            exit(0);
        }
        if(p == 0) {
            exit(0);
        }
    }

    sleep(1000);
}
