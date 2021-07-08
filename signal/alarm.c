#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void alarm_handler(int sign) {
    printf("alarm\n");
    fflush(stdout);
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(5);
    sleep(100);
    exit(0);
}