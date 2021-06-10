#include <stdio.h>
#include <stdlib.h>
#include <grp.h>

// print group name by group id
int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print group name by group id\nUsage: %s group id\n", argv[0]);
        exit(1);
    }

    int gid = atoi(argv[1]);
    struct group *g = getgrgid(gid);
    if(g == NULL) {
        perror("getgrgid()");
        exit(1);
    }

    fprintf(stdout, "group name = %s for gid %d\n", g->gr_name, gid);
    return 0;
}
