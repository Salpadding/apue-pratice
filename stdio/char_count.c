// count size of a regular file
// count.o filename
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "count file size of a regular file:\n%s filename\n", argv[0]);
        exit(0);
    }

    FILE * file = fopen(argv[1], "r");
    if(file == NULL) {
        perror("fopen()");
        return 0;
    }

    int count = 0;
    int c = 0;

    while(1) {
        c = fgetc(file);
        if(c == EOF)
            break;
        count++;
    }

    printf("size of %s is %d byte\n", argv[1], count);
    fclose(file);
    return 0;
}