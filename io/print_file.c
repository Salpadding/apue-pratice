// print content of a file

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 32

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print content of a file\n%s filename\n", argv[0]);
        exit(0);
    }

    FILE * file = fopen(argv[1], "r");
    if(file == NULL) {
        perror("fopen");
        exit(0);
    }

    char buf[BUF_SIZE];

    while(1) {
        if(fgets(buf, BUF_SIZE, file) == NULL) 
            break;
        fputs(buf, stdout);
    }

    fclose(file);
    return 0;
}