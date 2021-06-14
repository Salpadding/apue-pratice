#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TIME_BUF_SIZE 32

// print current time
int main(int argc, char ** argv) {
    char buf[TIME_BUF_SIZE];
    time_t now = time(NULL);

    if(now < 0) {
        perror("time()");
        exit(1);
    }

    struct tm* t = localtime(&now);

    if(strftime(buf, TIME_BUF_SIZE, "%Y-%m-%d %H:%M:%S\n", t) == 0) {
        fprintf(stderr, "error!\n");
        exit(1);
    }

    fprintf(stdout, "now = %s\n", buf);
}
