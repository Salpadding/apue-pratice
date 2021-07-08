#include "./mytbf.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MAX_TBF 1024

struct mytbf_st {
    int cps;
    int burst;
    int token;
    int pos;
};

struct mytbf_st* jobs[MAX_TBF];

static volatile int inited = 0;

typedef void (*sig_handler)(int);

static volatile sig_handler func;

// alarm signal handler
static void handle(int sig) {
    alarm(1);
    for(int i = 0; i < MAX_TBF; i++) {
        struct mytbf_st* p = jobs[i];
        if(p != NULL)
            p->token += p->cps;
    }
}

static void mytbf_load() {
    if(inited)
        return;
    inited = 1;        
    func = signal(SIGALRM, handle);
    alarm(1);
}

static void mytbf_unload() {
    if(!inited)
        return;
    inited = 0;    
    // cancel previous signal
    alarm(0);   
    if(func != NULL) 
        signal(SIGALRM, func);
    func = NULL;
    for(int i = 0; i < MAX_TBF; i++)
        free(jobs[i]);
}

#define MIN(x, y) (x < y ? x : y)



static int get_pos() {
    for(int i = 0; i < MAX_TBF; i++) {
        if(jobs[i] == NULL)
            return i;
    }
    return -1;
}

mytbf_t *mytbf_init(int cps,int burst) {
    mytbf_load();
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
