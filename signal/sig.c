#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void int_handler(int sign) {
    printf("SIG_INT!\n");
    fflush(NULL);
}

int main() {
    // ignore SIG_INT which send by ctrl+c
    signal(SIGINT, SIG_IGN);

    signal(SIGINT, int_handler);
    for(int i = 0; i < 10; i++) {
        write(1, "*", 1);
        sleep(1);
    }
    return 0;
}