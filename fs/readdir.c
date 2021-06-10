#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

// print files in directory

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print files in a directory\nUsage: %s dirname\n", argv[0]);
        exit(1);
    }

    DIR * dir = opendir(argv[1]);
    if(dir == NULL) {
        perror("opendir()");
        exit(1);
    }

    struct dirent * cur;
    while((cur = readdir(dir)) != NULL) {
        fprintf(stdout, "%s\n", cur->d_name);
    }

    closedir(dir);
    return 0;
}
