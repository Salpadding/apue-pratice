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


    for(int i = 0; i < 1000; i++) {
        // block int signal
        sigprocmask(SIG_BLOCK, &set, NULL);
        for(int j = 0; j < 5; j++) {
            write(1, "*", 1);
            sleep(1);
        }
        write(1, "\n", 1);
        // unblock int signal
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        // 这里可能信号可能已经响应完了，因为这两步不是原子性的
        pause();
    }

    // restore sigprocmask and signal handler
    sigprocmask(SIG_SETMASK, &old, NULL);
    signal(SIGINT, func);
    return 0;
}