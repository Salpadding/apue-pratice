#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(int argc, char** argv)
{
    // 以只读方式把文件映射到内存
    int fd = open(argv[1], O_RDONLY);

    if(fd < 0)
    {
        perror("open()");
        exit(1);
    }

    struct stat st;
    int err = fstat(fd, &st);

    if (err < 0)
    {
        perror("fstat()");
        exit(1);
    }

    char* s = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);

    if(s == MAP_FAILED)
    {
        perror("mmap()");
        exit(1);
    }

    for(ssize_t i = 0; i < st.st_size; i++)
    {
        fputc(s[i], stdout);
    }

    munmap(s, st.st_size);
    close(fd);

}