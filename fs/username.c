#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>

// print username by userid
int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print user name by user id\nUsage: %s user id\n", argv[0]);
        exit(1);
    }

    int uid = atoi(argv[1]);
    struct passwd *pwd = getpwuid(uid);
    if(pwd == NULL) {
        perror("getpwuid()");
        exit(1);
    }

    fprintf(stdout, "user name = %s for uid %d\n", pwd->pw_name, uid);
    return 0;
}
