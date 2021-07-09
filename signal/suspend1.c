#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static sigset_t empty;

static void suspend_handler(int sig) {
    exit(0);
}

static void int_handler(int sig) {

}

static void (*func)(int);

int main() {
    signal(SIGTSTP, suspend_handler);
    func = signal(SIGINT, int_handler);

    sigset_t old;
    sigset_t set;
    sigemptyset(&empty);
    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    // save sigprocmask 
    if(sigprocmask(SIG_BLOCK, &empty, &old) < 0) {
        perror("sigprocmask()");
        exit(0);
    }

    sigset_t oldset;
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    sigprocmask(SIG_BLOCK, &set, &oldset);

    for(int i = 0; i < 1000; i++) {
        // block int signal
        sigprocmask(SIG_BLOCK, &set, NULL);
        for(int j = 0; j < 5; j++) {
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        // 解除阻塞 因为 old set 没有阻塞 sigint，当收到信号后，恢复到 上一次 sigprocmask 的结果
        sigsuspend(&oldset);
    }

    // restore sigprocmask and signal handler
    sigprocmask(SIG_SETMASK, &old, NULL);
    signal(SIGINT, func);
    return 0;
}