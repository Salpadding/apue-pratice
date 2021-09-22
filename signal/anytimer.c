#include <stdio.h>
#include <stdlib.h>
#include "./anytimer.h"
#include <unistd.h>
#include <signal.h>

#define MAX_TASKS 1024
typedef void (*sig_handler)(int);

static volatile sig_handler hd = NULL;
static volatile int inited = 0;

static struct task *tasks[MAX_TASKS];

struct task
{
    int sec;
    at_job func;
    void *arg;
    int pos;
};

// handle per second
void handle(int sig)
{
    alarm(1);

    for (int i = 0; i < MAX_TASKS; i++)
    {
        struct task *t = tasks[i];
        if (t == NULL)
            continue;
        // substract the second value
        t->sec--;
        if (t->sec == 0)
        {
            t->func(t->arg);
            free(t);
            tasks[i] = NULL;
        }
    }
}

int getf_free_pos()
{
    for (int i = 0; i < MAX_TASKS; i++)
    {
        if (tasks[i] == NULL)
            return i;
    }
    return -1;
}

at_id at_add_job(int sec, at_job job, const void *arg)
{
    if (!inited)
    {
        inited = 1;
        signal(SIGALRM, handle);
        alarm(1);
    }

    at_id id = getf_free_pos();
    if (id < 0)
        return id;
    struct task *t = malloc(sizeof(struct task));
    t->arg = arg;
    t->func = job;
    t->pos = id;
    t->sec = sec;

    tasks[id] = t;
    return id;
}

int at_cancel_job(at_id id)
{
    struct task *t = tasks[id];
    if (t == NULL)
        return 0;
    free(t);
    tasks[id] = NULL;
}

// wait for job
void at_wait(at_id id)
{
    while (tasks[id] != NULL)
        pause();
}
