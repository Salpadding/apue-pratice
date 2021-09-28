#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MSG "hello pthread\n"

void* func(void* arg) 
{
    write(1, MSG, strlen(MSG));
    pthread_exit(NULL);
}

int main()
{
    pthread_t tid;
    int err;

    puts("Begin!");
    err = pthread_create(&tid, NULL, func, NULL);

    if(err)
    {
        fprintf(stderr, "pthread_create(): %s\n", strerror(err));
        exit(1);
    }

    err = pthread_join(tid, NULL);

    if(err)
    {
        fprintf(stderr, "pthread_join(): %s\n", strerror(err));
        exit(1);
    }

    puts("End!");
    exit(0);
}