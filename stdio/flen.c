// get file size of a regular file
// flen.o filename

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print file size of a regular file\nuse: %s filename\n", argv[0]);
        exit(0);
    }

    FILE * fp = fopen(argv[1], "r");
    if(fp == NULL) {
        perror("fopen");
        exit(0);
    }

    if(fseek(fp, 0, SEEK_END) < 0) {
        perror("fseek");
        fclose(fp);
        exit(0);
    }

    long sz = ftell(fp);
    fprintf(stdout, "size of %s is %ld\n", argv[1], sz);
    fclose(fp);
}