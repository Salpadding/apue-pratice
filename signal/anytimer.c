#include <stdio.h>
#include <stdlib.h>
#include "./anytimer.h"
#include <unistd.h>
#include <signal.h>

#define MAX_TASKS 1024
typedef void (*sig_handler) (int);

static volatile sig_handler hd = NULL;
static volatile int inited = 0;

static struct task* tasks[MAX_TASKS];

struct task {
    int sec;
    at_job func;
    const void* arg;
    int pos;
};

void handle(int sig) {
    alarm(1);

    for(int i = 0; i < MAX_TASKS; i++) {
        struct task* t = tasks[i];
        if(t == NULL)
            continue;
        t->sec--;
        if(t->sec == 0) {
            t->func(t->arg);
            free(t);
            tasks[i] = NULL;
        }
    }
}

int get_pos() {
    for(int i = 0; i < MAX_TASKS; i++) {
        if(tasks[i] == NULL)
            return i;
    }
    return -1;
}

at_id at_add_job(int sec, at_job job, const void * arg) {
    if(!inited) {
        inited = 1;
        signal(SIGALRM, handle);
        alarm(1);
    }

    at_id id = get_pos();
    if(id < 0)
        return id;
    struct task* t = malloc(sizeof(struct task));
    t->arg = arg;
    t->func = job;
    t->pos = id;
    t->sec = sec;

    tasks[id] = t;
    return id;
}

int at_cancel_job(at_id id) {
    return 0;
}

void at_wait(at_id id) {
    while(tasks[id] != NULL)
        pause();
}
