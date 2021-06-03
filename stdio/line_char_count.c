#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "count chars line by line\nusage %s filename\n", argv[0]);
        exit(0);
    }

    FILE * f = fopen(argv[1], "r");

    if(f == NULL) {
        perror("fopen()");
        exit(0);
    }

    char * line_ptr = NULL;
    size_t sz = 0;
    ssize_t r = 0;
    int line = 1;
    while (r >= 0)
    {
        r = getline(&line_ptr, &sz, f);
        // count chars here
        printf("chars at line %d = %ld\n", line, strlen(line_ptr));

        // free memory
        free(line_ptr);
        line_ptr = NULL;
        sz = 0;
        line++;
    }
    


    fclose(f);
    return 0;
}