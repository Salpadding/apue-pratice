#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static FILE* dst = NULL;

static void on_exit() {
    if(dst == NULL)
        return;
    
    fclose(dst);
    exit(0);
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "write a '*' charater to file per second, Usage: %s filename\n", argv[0]);
        exit(0);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    sigaction(SIGINT, &sa, NULL);
}