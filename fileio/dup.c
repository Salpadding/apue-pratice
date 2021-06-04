// redirect by dup

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>

int main() {
    // redirect from stdout to file
    close(1);
    int fd = open("log", O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(fd < 0) {
        perror("open()");
        exit(1);
    }
    puts("hello\n");
}