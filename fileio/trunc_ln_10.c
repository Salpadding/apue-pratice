// delete the 10th line of a text file

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define BUF_SIZE 256

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "delete the 10th line of a text file\nUsage: %s filename\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);

    // create tmp file w+
    FILE * tmp = tmpfile();

    if(fd < 0) {
        perror("open()");
        exit(1);
    }

    int line = 1; // line number
    char buf[BUF_SIZE];

    while(1) {
        ssize_t n = read(fd, buf, BUF_SIZE);
        if(n == 0)
            break;

        if(n < 0) {
            perror("read()");
            close(fd);
            exit(1);
        }

        for(ssize_t p = 0; p < n; p++) {
            if(line != 10)
                fputc(buf[p], tmp);
            if(buf[p] == '\n')
                line++;                
        }
    }

    fflush(tmp);
    close(fd);
    // remove to start
    fseeko(tmp, 0, SEEK_SET);

    FILE * f = fopen(argv[1], "w");
    if(f == NULL) {
        perror("fopen()");
        exit(1);
    }

    while(1) {
        char c = fgetc(tmp);
        if(c == EOF)
            break;

        if(fputc(c, f) == EOF) {
            perror("fputc()");
            fclose(f);
            fclose(tmp);            
            exit(1);
        }
    }

    fclose(f);
    fclose(tmp);
}
