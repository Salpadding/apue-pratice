#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    alarm(5);
    pause();
    printf("hello world\n");
    fflush(stdout);
}
