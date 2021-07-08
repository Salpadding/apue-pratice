#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define TOKEN_MAX 1024
#define CPS 10

static volatile unsigned int token = 0;

void on_alarm(int sig) {
    alarm(1);
    if(token >= TOKEN_MAX)
        return;
    token++;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "print content of file to standard output, Usage: %s filename\n", argv[1]);
        exit(1);
    }

    int src = open(argv[1], O_RDONLY);

    if(src < 0) {
        perror("fopen()");
        exit(1);
    }

    char buf[CPS];
    signal(SIGALRM, on_alarm);
    alarm(1);

    while(1) {
        while(token <= 0)
            pause();

        ssize_t sz = read(src, buf, CPS);

        if(sz < 0) {
            if(errno == EINTR)
                continue;
            perror("read()");
            exit(1);
        }

        token--;

        // eof
        if(sz == 0) {
            break;
        }

        ssize_t cur = 0;
        while(sz > 0) {
            ssize_t r = write(1, buf + cur, sz);

            if(r < 0) {
                if(errno == EINTR) {
                    continue;
                }
                perror("write()");
                exit(1);
            }
            cur += r;
            sz -= r;
        }
    }

    close(src);
    exit(0);
}