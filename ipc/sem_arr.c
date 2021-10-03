#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#define KEY "/etc/services"
#define PROJECT_ID 1330
#define TRUE 1
#define FALSE 0
#define THREADS 32

static volatile int count = 0;

// p 或者 v 操作, 占据或释放资源
static void p_v(int semid, int p)
{
    struct sembuf op;
    // 信号量所在的数组下标
    op.sem_num = 0;
    // 使用一个资源
    op.sem_op = p ? -1 : 1;
    // 特殊要求
    op.sem_flg = 0;

    // 这里 &op 应该是一个数组,但是因为目前数组只有一个成员,所以可以传结构体的指针
    while (semop(semid, &op, 1) < 0)
    {
        if (errno == EINTR || errno == EAGAIN)
        {
            continue;
        }
        perror("semop()");
        exit(1);
    }
}

static void* add(void *semid)
{
    int *id = semid;
    // 1. p
    p_v(*id, TRUE);
    count++;
    p_v(*id, FALSE);

    pthread_exit(NULL);
}

// semaphore, 可以用于控制并发数量
int main()
{
    // 使用父子间进程通信时, 可以用一个特殊的key值, IPC_PRIVATE
    int semid = semget(IPC_PRIVATE, 1, 0600);
    if (semid < 0)
    {
        perror("semid()");
        exit(1);
    }

    // 对信号量进行初始化, 把并发数量限制为 1
    if (semctl(semid, 0, SETVAL, 1) < 0)
    {
        perror("semctl()");
        exit(1);
    }

    pthread_t threads[THREADS];

    for (int i = 0; i < THREADS; i++)
    {
        int err = pthread_create(threads + i, NULL, add, &semid);
        if (err)
        {
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }

    for (int i = 0; i < THREADS; i++)
    {
        int err = pthread_join(threads[i], NULL);
        if (err)
        {
            fprintf(stderr, "pthread_join(): %s\n", strerror(err));
            exit(1);
        }
    }

    printf("count = %d\n", count);

    // 销毁信号量数组
    semctl(semid, 0, IPC_RMID);
    exit(0);
}