// implement copy by fileio
// mycpy sourcefile destfile
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 2048

void write_buf(const char * buf, ssize_t sz, int fd) {
    ssize_t remain = sz;
    while(remain != 0) {
        remain -= write(fd, buf + sz - remain, remain);
    }
}

int main(int argc, char ** argv) {
    if(argc < 3) {
        fprintf(stderr, "copy file\nUsage: %s src dst\n", argv[0]);
        exit(0);
    }

    // open src 
    int src = open(argv[1], O_RDONLY);
    if(src < 0) {
        perror("open()");
        exit(0);
    }

    // open dst
    int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0744);
    if(dst < 0) {
        perror("open()");
        close(src);
        exit(0);
    }

    char buf[BUF_SIZE];
    ssize_t sz;
    while(1) {
        sz = read(src, buf, BUF_SIZE);

        if(sz < 0) {
            perror("read()");
            close(src);
            close(dst);
            exit(0);
        }

        if(sz == 0)
            break;

        write_buf(buf, sz, dst);
    }

    close(src);
    close(dst);
}