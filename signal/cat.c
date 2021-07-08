#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define CPS 10

// source fd
static volatile int src = -1;

void handle(int sig) {
    char buf[CPS];
    int eof = read(src, buf, CPS);
    if(eof < 0) {
        if(errno == EINTR)
            return;

        close(src);
        exit(0);
    }

    for(int i = 0; i < eof; i++) {
        if(fputc(buf[i], stdout) >= 0) {
            continue;
        }
        if(errno == EINTR) 
            continue;
        perror("fputc()");
        exit(1);
    }
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print content of file to standart output, Usage: %s filename\n", argv[0]);
        exit(1);
    }

    src = open(argv[1], O_RDONLY);
    if(src < 0) {
        perror("open()\n");
        exit(1);
    }

    signal(SIGALRM, handle);
    alarm(1);
    while(1) {
        pause();
        alarm(1);
    }
}