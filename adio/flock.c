#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define COUNT 64
#define BUF_SIZE 4096

int main(int argc, char** argv)
{
    char buf[BUF_SIZE];
    for(int i = 0; i < COUNT; i++)
    {
        pid_t pid = fork();
        
        if(pid < 0)
        {
            perror("fork()");
            exit(1);
        }

        if(pid == 0)
        {
            // child
            int fd = open(argv[1], O_RDWR | O_CREAT, 0600);

            if(fd < 0)
            {
                perror("open()");
                exit(1);
            }

            // lock 
            if(lockf(fd, F_LOCK, 0) < 0) {
                perror("lockf()");
                exit(1);
            }


            // read into buffer 
            if( read(fd, buf, BUF_SIZE) < 0) {
                perror("read()");
                exit(1);
            }

            int i = atoi(buf);
            i++;

            int sz = snprintf(buf, BUF_SIZE, "%d", i);

            lseek(fd, 0, SEEK_SET);
            write(fd, buf, sz);

            // unlock
            if(lockf(fd, F_ULOCK, 0) < 0){
                perror("lockf()");
                exit(1);
            }

            // close fd
            close(fd);
            exit(0);
        }
    }

    // wait for children
    for(int i = 0; i < COUNT; i++)
    {
        wait(NULL);
    }
    
    exit(0);
}