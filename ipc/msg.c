// 消息队列使用
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
#define MESSAGE "hello mq"

int create_mq()
{
    key_t k = ftok(KEY, PROJECT_ID);

    if(k < 0)
    {
        perror("ftok()");
        exit(1);
    }

    int id = msgget(k, IPC_CREAT | 0600);

    if(id < 0)
    {
        perror("msgget()");
        exit(1);
    }

    return id;
}

int main()
{
    char buf[BUF_SIZE];
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork()");
        exit(1);
    }

    if(pid == 0) 
    {
        ssize_t sz;
        // child process, read from msg queue
        int mq = create_mq();

        while(1)
        {
            // msgtyp 可以挑选消息来接收
            // msgflg 可以选择异步的方式,或者截断
            sz = msgrcv(mq, buf, BUF_SIZE, 0, 0);

            if(sz < 0)
            {
                perror("msgrcv()");
                break;
            }

            printf("message received: %s\n", buf);
        }
        msgctl(mq, IPC_RMID, NULL);
    }
    else 
    {
        int mq = create_mq();
        while(1)
        {
            if(msgsnd(mq, MESSAGE, strlen(MESSAGE), 0) < 0)
                break;
            sleep(1);
        }
        msgctl(mq, IPC_RMID, NULL);
    }

    exit(0);
}