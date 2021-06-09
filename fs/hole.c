// create a 1G hole file filled with '\0'
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "create a 1G hole file filled with '\\0'\nUsage %s file name\n", argv[0]);
        exit(1);
    }   

    int fd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(fd < 0) {
        perror("open()");
        exit(1);
    }

    long long k = 1024;
    long long file_size = k * k * k; 
    lseek(fd, file_size - 1, SEEK_SET);
    write(fd, "\0", 1);
    close(fd);
}
