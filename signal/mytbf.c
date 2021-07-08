#include "./mytbf.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_TBF 1024

struct mytbf_st* jobs[MAX_TBF];

static int inited = 0;

static void (*func)(int);

#define MIN(x, y) (x < y ? x : y)

struct mytbf_st {
    int cps;
    int burst;
    int token;
    int pos;
};

static int get_pos() {
    for(int i = 0; i < MAX_TBF; i++) {
        if(jobs[i] == NULL)
            return i;
    }
    return -1;
}

// alarm signal handler
static void handle(int sig) {
    alarm(1);
    for(int i = 0; i < MAX_TBF; i++) {
        struct mytbf_st* p = jobs[i];
        if(p != NULL)
            p->token += p->cps;
    }
}

mytbf_t *mytbf_init(int cps,int burst) {
    if(!inited) {
        func = signal(SIGALRM, handle);
        alarm(1);
        inited = 1;
    }

    struct mytbf_st* m = malloc(sizeof(struct mytbf_st));

    if(m == NULL)
        return NULL;

    int i = get_pos();
    if(i < 0)
        return NULL;

    m->cps = cps;
    m->burst = burst;
    m->token = 0;
    m->pos = i;
    jobs[i] = m;

    return m;
}

int mytbf_returntoken(mytbf_t * bf, int token) {
    struct mytbf_st* p = bf;
    p->token += token;
    p->token = MIN(p->token, p->burst);
    return token;
}

int mytbf_fetchtoken(mytbf_t * bf, int token) {
    struct mytbf_st* p = bf;
    while(p->token == 0) {
        pause(); // 等待 alarm 信号打断阻塞
    }    

    int r = MIN(p->token, token);
    p->token -= r;
    return r;
}

int mytbf_destroy(mytbf_t * bf) {
    struct mytbf_st* p = bf;
    jobs[p->pos] = NULL;
    free(p);
    return 1;
}
