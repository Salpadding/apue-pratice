#include "./mytbf.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

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


// initialize this library
static void mytbf_load() {
    if(inited)
        return;
    inited = 1;        
    alarm(1);

    // save the original sigalrm handler
    func = signal(SIGALRM, handle);
}

static void mytbf_unload() {
    if(!inited)
        return;
    inited = 0;    
    // cancel previous signal
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

int mytbf_init(int cps,int burst) {
    mytbf_load();
    struct mytbf_st* m = malloc(sizeof(struct mytbf_st));

    if(m == NULL)
        return -1;

    int i = get_pos();
    if(i < 0)
        return -1;

    m->cps = cps;
    m->burst = burst;
    m->token = 0;
    m->pos = i;
    jobs[i] = m;

    return i;
}

int mytbf_returntoken(int id, int token) {
    struct mytbf_st* p = jobs[id];
    p->token += token;
    p->token = MIN(p->token, p->burst);
    return token;
}

int mytbf_fetchtoken(int id, int token) {
    struct mytbf_st* p = jobs[id];
    while(p->token == 0) {
        pause(); // blocked until sigalrm received
    }    

    int r = MIN(p->token, token);
    p->token -= r;
    return r;
}

int mytbf_destroy(int id) {
    struct mytbf_st* p = jobs[id];
    jobs[p->pos] = NULL;
    free(p);
    return 1;
}
