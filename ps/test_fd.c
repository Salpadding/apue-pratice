#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <wait.h>

// parent: write hello\n
// child: write world\n
int main(int argc, char** argv)
{
    if(argc != 2) 
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, 0666);

    if(fd < 0) 
    {
        perror("open");
        exit(1);
    }

    // the new fd should be 3
    printf("new fd = %d created\n", fd);

    write(fd, "hello\n", strlen("hello\n"));

    pid_t pid = fork();

    if(pid == 0)
    {
        write(3, "world\n", strlen("world\n"));
        exit(0);
    }

    wait(NULL);
}