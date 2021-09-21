#include <stdio.h>
#include <stdlib.h>

extern char ** environ;

int main(int argc, char ** argv) {
    for(int i = 0; environ[i] != NULL; i++) {
        fputs(environ[i], stdout);
        fputc('\n', stdout);
    }
    return 0;
} 