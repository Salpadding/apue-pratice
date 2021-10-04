// 使用 select 进行批量复制
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 128

void my_sleep(int seconds)
{
    struct timeval t;
    // 这里一定要全部赋值,不然会引用未初始化的内存
    t.tv_sec = seconds;
    t.tv_usec = 0;
    select(0, NULL, NULL, NULL, &t);
}

// current state of pipeline
enum pipe_state
{
    STATE_READ = 0,
    STATE_WRITE,
    STATE_ERR,
    STATE_EOF
};

// context for copy from src to dst
struct pipe_st
{
    // src file descriptor
    int src;
    // dst file descriptor
    int dst;
    // current state
    int state;
    char buf[BUF_SIZE];
    // offset for write
    ssize_t off;
    // length of buffer
    ssize_t len;
    // error number
    int err;
};

// the file descriptor should be opened async
// return -errno if failed
int pipe_init(struct pipe_st *p, int src, int dst)
{
    p->src = src;
    p->dst = dst;
    p->off = 0;
    p->len = 0;
    p->err = 0;
    p->state = STATE_READ;
}

// push state machine
// return -errno if error found
// return 0 if eof or success
int pipe_push(struct pipe_st *p, int n)
{
    printf("pipe push()\n");
    // create file descriptor sets
    fd_set reads;
    fd_set writes;
    fd_set excepts;
    FD_ZERO(&reads);
    FD_ZERO(&writes);
    FD_ZERO(&excepts);
    int nfds = 0;

    for (int i = 0; i < n; i++)
    {
        if (p->src > nfds)
            nfds = p->src;
        if (p->dst > nfds)
            nfds = p->dst;

        if (p[i].state == STATE_READ)
        {
            printf("%d.src %d to read set\n", i, p[i].src);
            FD_SET(p[i].src, &reads);
            FD_SET(p[i].src, &excepts);
        }

        if (p[i].state == STATE_WRITE)
        {
            printf("%d.dst %d to write set\n", i, p[i].dst);
            FD_SET(p[i].dst, &writes);
            FD_SET(p[i].dst, &excepts);
        }
    }

    int fds = 0;
    printf("try to select()\n");
    while (1)
    {
        // blocking until some file descriptor is ready
        fds = select(nfds + 1, &reads, &writes, &excepts, NULL);
        printf("fds = %d", fds);
        if (fds < 0)
        {
            if (errno == EINTR)
                continue;
            return -errno;
        }
        break;
    }

    printf("select result = %d\n", fds);
    ssize_t sz;
    for (int i = 0; i < n; i++)
    {
        printf("current i = %d\n", i);
        switch (p[i].state)
        {
        case STATE_READ:
            if (FD_ISSET(p[i].src, &excepts))
            {
                printf("i = %d in excepts\n", i);
                p[i].state = STATE_ERR;
                break;
            }
            if (!FD_ISSET(p[i].src, &reads))
            {
                printf("i = %d not in reads\n", i);
                break;
            }
            // read is available
            sz = read(p[i].src, p[i].buf, BUF_SIZE);
            printf("read sz %ld from i = %d\n", sz, i);
            if (sz == 0)
            {
                p[i].state = STATE_EOF;
                break;
            }

            p[i].state = STATE_WRITE;
            p[i].off = 0;
            p[i].len = sz;
            break;
        case STATE_WRITE:
            if (FD_ISSET(p[i].dst, &excepts))
            {
                printf("i = %d in excepts\n", i);
                p[i].state = STATE_ERR;
                break;
            }
            if (!FD_ISSET(p[i].dst, &writes))
            {
                printf("i = %d not in writes\n", i);
                break;
            }
            // write is available
            sz = write(p[i].dst, p[i].buf, p[i].len);
            printf("write success i = %d, len = %ld\n", i, sz);
            p[i].off += sz;
            if (p[i].off == p[i].len)
                p[i].state = STATE_READ;
            break;
        default:
            break;
        }
    }
    return 0;
}

// copy from src to dst by state machined
int main(int argc, char **argv)
{
    if (argc < 3 || ((argc - 1) % 2 != 0))
    {
        fprintf(stderr, "usage %s [src file] [dst file] ... \n", argv[0]);
        exit(1);
    }

    int pipes = ((argc - 1) / 2);
    printf("pipes = %d\n", pipes);
    struct pipe_st *ps = malloc(sizeof(struct pipe_st) * pipes);

    if (ps == NULL)
    {
        fprintf(stderr, "malloc\n");
        exit(1);
    }

    for (int i = 0; i < pipes; i++)
    {
        printf("source file = %s\n", argv[i * 2 + 1]);
        printf("dst file = %s\n", argv[i * 2 + 2]);
        int src = open(argv[i * 2 + 1], O_RDONLY | O_NONBLOCK);
        if (src < 0)
        {
            perror("open()");
            exit(1);
        }
        int dst = open(argv[i * 2 + 2], O_WRONLY | O_CREAT | O_NONBLOCK | O_TRUNC, 0600);
        if (dst < 0)
        {
            perror("open()");
            exit(1);
        }
        pipe_init(ps + i, src, dst);
    }


    while (1)
    {
        int err = pipe_push(ps, pipes);
        if (err)
        {
            fprintf(stderr, "%s\n", strerror(-err));
            exit(1);
        }

        int finished = 1;
        for (int i = 0; i < pipes; i++)
        {
            if (ps[i].state != STATE_EOF && ps[i].state != STATE_ERR)
            {
                finished = 0;
            }
        }

        if (finished)
            break;
    }

    for (int i = 0; i < pipes; i++)
    {
        close(ps[i].dst);
        close(ps[i].src);
    }

    exit(0);
}