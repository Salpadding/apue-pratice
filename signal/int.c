#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

typedef void (*handler_t) (int);

static void handler(int sig) {
    write(1, "!", 1);
}

int main() {
    handler_t p = signal(SIGINT, handler);


    sigset_t set;
    sigset_t old;
    sigset_t empty;

    sigemptyset(&empty);
    sigprocmask(SIG_BLOCK, &empty, &old);
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    
    // block interrupt signal

    for(int i = 0; i < 1000; i++) {
        // 被屏蔽的信号不会打断系统调用
        sigprocmask(SIG_BLOCK, &set, NULL);
        for(int j = 0; j < 5; j++) {
            write(1, "*", 1);
            sleep(1);
        }
        sigprocmask(SIG_UNBLOCK, &set, NULL);
        write(1, "\n", 1);
    }

    sigprocmask(SIG_SETMASK, &old, NULL);
    signal(SIGINT, p);
    return 0;
}