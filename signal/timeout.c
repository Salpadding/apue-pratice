#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

static int RUNNING = 1;
static int64_t count = 0;

static void alarm_handler(int sig) {
    RUNNING = FALSE;
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(5);
    fprintf(stdout, "running = %d\n", RUNNING);

    while(RUNNING == TRUE) {
        count++;
    }

    fprintf(stdout, "%lld\n", count);
}