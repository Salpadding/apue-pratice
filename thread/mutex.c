#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

// 使用静态初始化宏
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static volatile int count = 0;

#define assert(a, b) \
    if (a < 0)       \
    {                \
        perror(b);   \
        exit(1);     \
    }

#define THREADS 256

void *routine(void *args)
{
    assert(pthread_mutex_lock(&mtx), "pthread_mutex_lock()");
    count++;
    assert(pthread_mutex_unlock(&mtx), "pthread_mutex_unlock()");
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    pthread_t threads[THREADS];

    for(int i = 0; i < THREADS; i++)
    {
        assert(pthread_create(threads + i, NULL, routine, NULL), "pthread_create()");
    }
    
    for(int i = 0; i < THREADS; i++)
    {
        assert(pthread_join(threads[i], NULL), "pthread_join()");
    }

    printf("count = %d\n", count);
    exit(0);
}