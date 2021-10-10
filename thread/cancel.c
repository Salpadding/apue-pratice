// 线程取消
// man pthread_cancel
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

static volatile int i = 0;

void my_sleep(int seconds)
{
    struct timeval timeout;
    timeout.tv_usec = 0;
    timeout.tv_sec = seconds;
    select(0, NULL, NULL, NULL, &timeout);
}

void *routine(void *args)
{
    if(pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL) < 0) {
        perror("pthread_setcancelstate()");
        pthread_exit(NULL);
    }

    while (1)
    {
        i++;
        pthread_testcancel();
        printf("i = %d\n", i);
        my_sleep(1);
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;

    if (pthread_create(&tid, NULL, routine, NULL) < 0)
    {
        perror("pthread_create()");
        exit(1);
    }

    // wait for 5 second, then cancel
    my_sleep(5);

    if (pthread_cancel(tid) < 0)
    {
        perror("pthread_cancel()");
        exit(1);
    }

    printf("pthread_cancel() returned\n");

    void* ret;
    if (pthread_join(tid, &ret) < 0)
    {
        perror("pthread_join()");
        exit(1);
    }

    printf("pthread_join() returned\n");
    if(ret == PTHREAD_CANCELED)
    {
        printf("cancel\n");
    }

    printf("i = %d\n", i);
    exit(0);
}