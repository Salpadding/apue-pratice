#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#define MAX_TASKS 1024

struct thread_pool_st
{
    int size;
    int initialized;
    // pending tasks
    void (*tasks[MAX_TASKS])(void *);
    // arguments of pending tasks
    void *args[MAX_TASKS];
    // fixed thread array, length = size
    pthread_t *threads;
    pthread_mutex_t mtx;
    pthread_cond_t cond;

    pthread_mutex_t task_mtx;
    pthread_mutex_t waits[MAX_TASKS];
};

int find_task_lock(struct thread_pool_st *p)
{
    pthread_mutex_lock(&p->task_mtx);
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (p->tasks[i] != NULL)
        {
            pthread_mutex_unlock(&p->task_mtx);
            return i;
        }
    }
    pthread_mutex_unlock(&p->task_mtx);
    return -1;
}

void run_task_lock(struct thread_pool_st *p, int id)
{
    pthread_mutex_lock(&p->task_mtx);
    void (*task)(void *);
    task = p->tasks[id];
    void *args = p->args[id];
    p->args[id] = NULL;
    p->tasks[id] = NULL;
    pthread_mutex_unlock(&p->task_mtx);

    if (task == NULL)
        return;

    pthread_mutex_unlock(&p->mtx);

    // execute task
    task(args);

    pthread_mutex_unlock(p->waits + id);
}

void *thread_pool_routine(void *arg)
{
    struct thread_pool_st *p = arg;
    for (;;)
    {
        if (pthread_mutex_lock(&p->mtx) < 0)
        {
            perror("pthread_mutex_lock()");
            exit(1);
        }

        // printf("lock acq th routine\n");
        int tid;

        while ((tid = find_task_lock(p)) < 0)
        {
            // waiting for new task coming, and reacquire the lock
            // printf("waiting for condition\n");
            if (pthread_cond_wait(&p->cond, &p->mtx) < 0)
            {
                perror("pthread_cond_wait()");
                exit(1);
            }

            // printf("pthread_cond_wait ret\n");
        }

        // printf("tid >=0 found\n");
        // pop the task and execute
        run_task_lock(p, tid);
    }
}

void tp_init(struct thread_pool_st *pool, int threads)
{
    if (pool->initialized)
        return;

    pool->size = threads;

    for (int i = 0; i < MAX_TASKS; i++)
    {
        pool->tasks[i] = NULL;
        pool->args[i] = NULL;
        if (pthread_mutex_init(pool->waits + i, NULL) < 0)
        {
            perror("pthread_mutex_init()");
            exit(1);
        }
    }

    // initialize mutex
    if (pthread_mutex_init(&pool->mtx, NULL) < 0)
    {
        perror("pthread_mutex_init()");
        exit(1);
    }

    if (pthread_mutex_init(&pool->task_mtx, NULL) < 0)
    {
        perror("pthread_mutex_init()");
        exit(1);
    }

    // initialize condition
    if (pthread_cond_init(&pool->cond, NULL) < 0)
    {
        perror("pthread_cond_init()");
        exit(1);
    }

    pool->threads = malloc(sizeof(pthread_t) * pool->size);

    for (int i = 0; i < pool->size; i++)
    {
        if (pthread_create(pool->threads + i, NULL, thread_pool_routine, pool) < 0)
        {
            perror("pthread_create()\n");
        }
    }

    // waiting for thread created
    sleep(1);
    // create threads
    pool->initialized = 1;
}

int tp_add_task(struct thread_pool_st *pool, void (*task)(void *), void *arg)
{
    if (!pool->initialized)
    {
        fprintf(stderr, "pool not initialized\n");
        exit(1);
    }

    pthread_mutex_lock(&pool->task_mtx);
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (pool->tasks[i] == NULL)
        {
            if (pthread_mutex_lock(pool->waits + i) < 0)
            {
                perror("pthread_mutex_lock()");
            }

            pool->tasks[i] = task;
            pool->args[i] = arg;

            pthread_mutex_unlock(&pool->task_mtx);

            if (pthread_cond_broadcast(&pool->cond) < 0)
            {
                perror("pthread_cond_signal()");
                exit(1);
            }

            // printf("broadcast to threads i = %d != null = %d\n", i, pool->tasks[i] != NULL);

            return i;
        }
    }

    pthread_mutex_unlock(&pool->task_mtx);
    fprintf(stderr, "too much tasks\n");
    exit(1);
}

void tp_wait_task(struct thread_pool_st *pool, int id)
{
    if (pthread_mutex_lock(pool->waits + id) < 0)
    {
        perror("pthread_mutex_lock()");
    }

    pthread_mutex_unlock(pool->waits + id);
}

void tp_close(struct thread_pool_st *pool)
{
    // cancel threads
    for (int i = 0; i < pool->size; i++)
    {
        pthread_cancel(pool->threads[i]);
    }

    // free heap memory
    free(pool->threads);

    // destrory mutex
    pthread_mutex_destroy(&pool->mtx);
    pthread_cond_destroy(&pool->cond);

    for (int i = 0; i < MAX_TASKS; i++)
    {
        pthread_mutex_destroy(pool->waits + i);
    }
}

#define LEFT 30000
#define RIGHT 30200
#define TASKS (RIGHT - LEFT + 1)
#define THREADS 8

void routine(void *arg)
{
    int *i = arg;
    printf("i = %d pid = %ld \n", *i, pthread_self());

    for (int j = 2; j < *i; j++)
    {
        if (*i % j == 0)
        {
            return;
        }
    }

    // printf("%d is prime\n", *i);
}

int main()
{
    int args[TASKS];
    int tasks[TASKS];
    int j = 0;
    struct thread_pool_st tp;
    tp_init(&tp, THREADS);

    for (int i = LEFT; i <= RIGHT; i++)
    {
        args[j] = i;
        tasks[j] = tp_add_task(&tp, routine, args + j);
        j++;
    }

    for (int i = 0; i < TASKS; i++)
    {
        tp_wait_task(&tp, tasks[i]);
    }

    tp_close(&tp);
    exit(0);
}
