#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


static void my_on_exit(int sig) {
    exit(0);
}

static void my_action(int sig, siginfo_t* info, void * _) {
    // 
    if(info->si_code == SI_KERNEL) {
        exit(0);
    }
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "write a '*' charater to file per second, Usage: %s filename\n", argv[0]);
        exit(0);
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    // block SIGINT when handler triggered
    sigaddset(&sa.sa_mask, SIGINT);
    sa.sa_flags = 0;
    sa.sa_sigaction = my_on_exit;
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGINT, &sa, NULL);

    printf("pid = %d\n", getpid());
    while(1) {
	printf("running..\n");
	sleep(1);
    }
}
