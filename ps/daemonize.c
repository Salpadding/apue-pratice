#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <syslog.h>

// for ubuntu: cat /var/log/syslog | grep mydaemon
#define IDENT "mydaemon"

int main()
{
    pid_t pid = fork();

    if(pid > 0)
        exit(0);

    // redirect 0,1,2 to /dev/null

    int null_fd = open("/dev/null", O_WRONLY | O_APPEND);
    dup2(null_fd, 1);
    dup2(null_fd, 2);
    dup2(null_fd, 0);
    
    // detach from controlling terminal
    setsid();
    chdir("/");
    openlog(IDENT, LOG_PID, LOG_DAEMON);

    for(int i =0; ;i++)
    {
        syslog(LOG_INFO, "i = %d\n", i);
        sleep(1);
    }

    closelog();
    return 0;
}