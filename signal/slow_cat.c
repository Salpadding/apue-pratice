#include <stdio.h>
#include <stdlib.h>
#include "./mytbf.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#define CPS 10
#define BURST 1024
#define BUF_SIZE 1024


int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "print content of file into standard output, Usage: %s filename\n", argv[0]);
        exit(1);
    }

    int src = open(argv[1], O_RDONLY);
    if(src < 0) {
        perror("open()");
        exit(1);
    }

    mytbf_t* p = mytbf_init(CPS, BURST);
    static char buf[BUF_SIZE];

    if(p == NULL) {
        fprintf(stderr, "create job failed\n");
        exit(1);
    }

    while (1)
    {
        // 进入阻塞 等待信号到来
        int fetched = mytbf_fetchtoken(p, BUF_SIZE);

        if(fetched == 0)
            continue;

        ssize_t sz = read(src, buf, fetched);

        if(sz == 0)
            break;

        if(sz < 0) {
            if(errno == EINTR) {
                continue;
            }
            perror("read()");
            exit(1);
        }
        mytbf_returntoken(p, fetched - sz);

        ssize_t cur = 0;
        while(cur != sz) {
            ssize_t writed = write(1, buf + cur, sz);

            if(writed < 0) {
                if(errno == EINTR)
                    continue;
                perror("write()");
                exit(1);
            }

            cur += writed;
        }
    }
    
    mytbf_destroy(p);
}