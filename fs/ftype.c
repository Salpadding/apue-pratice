#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

// print the file type
//                           0123456789abcdef
static const char * types = "*pc*d*b*-*l*s***";

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print file type\nUsage: %s filename\n", argv[0]);
        exit(1);
    }
    struct stat st;
    if(stat(argv[1], &st) < 0) {
        perror("stat()");
        exit(1);
    }

    unsigned int mask =  ((unsigned int) st.st_mode) & S_IFMT;
    mask = mask >> 12;

    if(mask >= strlen(types)) {
        fprintf(stderr, "buffer overflow\n");
        exit(1);
    }

    fprintf(stdout, "type of file %s is %c\n", argv[1], types[mask]);
}
