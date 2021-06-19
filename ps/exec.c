#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Begin!\n");
    fflush(NULL);

    execl("/usr/bin/date", "date", "+%s", NULL);
    perror("execl");
}