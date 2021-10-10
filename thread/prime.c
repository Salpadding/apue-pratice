#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define LEFT 30000
#define RIGHT 30200
#define THREADS (RIGHT - LEFT + 1)

void *routine(void *arg)
{
    int *i = arg;

    for (int j = 2; j < *i; j++)
    {
        if (*i % j == 0)
        {
            pthread_exit(NULL);
        }
    }

    printf("%d is prime\n", *i);
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    pthread_t threads[THREADS];
    int args[THREADS];
    int j = 0;
    for (int i = LEFT; i <= RIGHT; i++)
    {
        args[j] = i;
        if (pthread_create(threads + j, NULL, routine, args + j) < 0)
        {
            perror("pthread_create()");
            exit(1);
        }
        j++;
    }

    for (int i = 0; i < THREADS; i++)
    {
        if (pthread_join(threads[i], NULL) < 0)
        {
            perror("pthread_join()");
            exit(1);
        }
    }

    exit(0);
}