// 共享内存的使用
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define BUF_SIZE 1024
#define KEY "/etc/services"
#define PROJECT_ID 1330
#define MSG_TPL "hello mq %d"

static volatile int count = 0;

// 创建消息队列
int create_shm()
{

    key_t k = ftok(KEY, PROJECT_ID);

    if (k < 0)
    {
        perror("ftok()");
        exit(1);
    }

    int id = shmget(k, BUF_SIZE, IPC_CREAT | 0600);

    if (id < 0)
    {
        perror("shmget()");
        exit(1);
    }

    return id;
}

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process
        // print share memory
        int shmid = create_shm();
        char *mem = shmat(shmid, NULL, 0);

        if (mem == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }

        while (1)
        {
            printf("content of memory = %s\n", mem);
            sleep(1);
        }

        shmdt(mem);
        shmctl(shmid, IPC_RMID, NULL);
    }
    else
    {
        // parent process
        // write share memory
        int shmid = create_shm();
        char *mem = shmat(shmid, NULL, 0);

        if (mem == (void *)-1)
        {
            perror("shmat()");
            exit(1);
        }

        while (1)
        {
            count++;
            snprintf(mem, BUF_SIZE, MSG_TPL, count);
            sleep(1);
        }

        shmdt(mem);
        shmctl(shmid, IPC_RMID, NULL);
    }
}