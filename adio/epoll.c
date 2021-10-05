// 使用 poll 进行批量复制
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/epoll.h>

#define BUF_SIZE 4096

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
    // custom data
    void *ptr;
};

// the file descriptor should be opened async
// return -errno if failed
void pipe_init(struct pipe_st *p, int src, int dst)
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
    printf("pipe push\n");
    // since Linux 2.6.8, size is ignored
    int ep = epoll_create(1);

    if (ep < 0)
    {
        perror("epoll()");
        return -errno;
    }

    for (int i = 0; i < n; i++)
    {
        int *ptr = malloc(sizeof(int));
        *ptr = i;
        p[i].ptr = ptr;

        if (p[i].state == STATE_READ)
        {
            struct epoll_event e;
            e.events = EPOLLIN | EPOLLERR;
            e.data.fd = p[i].src;
            // 传一个指针用于接收
            e.data.ptr = p + i;
            epoll_ctl(ep, EPOLL_CTL_ADD, p[i].src, &e);
            printf("add %d.src = %d to reads\n", i, p[i].src);
        }
        if (p[i].state == STATE_WRITE)
        {
            struct epoll_event e;
            e.events = EPOLLOUT | EPOLLERR;
            e.data.fd = p[i].dst;
            e.data.ptr = p + i;
            epoll_ctl(ep, EPOLL_CTL_ADD, p[i].dst, &e);
            printf("add %d.dst = %d to writes\n", i, p[i].dst);
        }
    }

    // wait for some file descriptor is ready

    int fds = 0;
    struct epoll_event *evs = malloc(sizeof(struct epoll_event) * n * 2);
    while ((fds = epoll_wait(ep, evs, n * 2, -1)) < 0)
    {
        if (errno != EINTR)
        {
            return -errno;
        }
    }

    printf("ready fds = %d\n", fds);

    ssize_t sz;
    for (int i = 0; i < fds; i++)
    {
        struct pipe_st *pi = evs[i].data.ptr;
        int *j = pi->ptr;

        switch (pi->state)
        {
        case STATE_READ:
            if (EPOLLERR & evs[i].events)
            {
                printf("%d.src %d %d in excepts\n", *j, pi->src, evs[i].data.fd);
                pi->state = STATE_ERR;
                break;
            }
            if ((EPOLLIN & evs[i].events) == 0)
            {
                printf("%d.src %d %d not in reads\n", *j, pi->src, evs[i].data.fd);
                break;
            }
            // read is available
            printf("%d.src %d %d in reads\n", *j, pi->src, evs[i].data.fd);
            sz = read(pi->src, pi->buf, BUF_SIZE);
            printf("read sz %ld from i = %d\n", sz, i);
            if (sz == 0)
            {
                pi->state = STATE_EOF;
                break;
            }

            pi->state = STATE_WRITE;
            pi->off = 0;
            pi->len = sz;
            break;
        case STATE_WRITE:
            if (EPOLLERR & evs[i].events)
            {
                printf("%d.dst %d %d in excepts\n", *j, pi->dst, evs[i].data.fd);
                pi->state = STATE_ERR;
                break;
            }
            if ((EPOLLIN & evs[i].events) == 0)
            {
                printf("%d.dst %d %d not in writes\n", *j, pi->dst, evs[i].data.fd);
                break;
            }
            // write is available
            printf("%d.dst %d %d in writes\n", *j, pi->dst, evs[i].data.fd);
            sz = write(pi->dst, pi->buf, pi->len);
            printf("write success i = %d, len = %ld\n", *j, sz);
            pi->off += sz;
            if (pi->off == pi->len)
                pi->state = STATE_READ;
            break;
        default:
            break;
        }
    }

    close(ep);

    for (int i = 0; i < n; i++)
        free(p[i].ptr);
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

    int limits = 20;
    int j = 0;
    while ((j++) < limits)
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

    printf("close file descriptors\n");
    for (int i = pipes - 1; i >= 0; i--)
    {
        close(ps[i].dst);
        close(ps[i].src);
    }

    printf("free pipe memory\n");
    free(ps);
    printf("free pipe memory success\n");
    exit(0);
}