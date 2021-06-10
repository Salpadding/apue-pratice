#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>

// print files matched

int err_handler(const char * err_path, int errno) {
    fprintf(stderr, "error path = %s reason = %s", err_path, strerror(errno));
    return 0;
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print files matching pattern\nUsage: %s pattern\n", argv[0]);
        exit(1);
    }

    glob_t g;

    if(glob(argv[1], 0, err_handler, &g) != 0){
        perror("glob()");
        exit(1);
    }
    for(int i = 0; i < g.gl_pathc; i++) {
        fprintf(stdout, "%s\n", g.gl_pathv[i]);
    }
    globfree(&g);
    return 0;
}
