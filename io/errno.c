
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main() {
    FILE *fp;
    fp = fopen("stdio.c", "r");

    if(fp == NULL) {
        fprintf(stderr, "fopen() failed errno = %d\n", errno);
        return 0;
    }

    fclose(fp);
    return 0;
}
