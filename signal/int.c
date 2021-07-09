#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static void handler(int sig) {
    write(1, "!", 1);
}

int main() {
    signal(SIGINT, handler);
    sigset_t set;
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

    return 0;
}