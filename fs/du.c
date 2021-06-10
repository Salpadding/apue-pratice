// print directory/file disk usage recursively
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

static const char * units = "bkmg";
static const double k = 1024.0;

uint64_t du(const char * path) {
    uint64_t r = 0;
    struct stat st;
    if(lstat(path, &st) < 0) {
        perror("lstat");
        exit(1);
    }

    r += st.st_blocks * 512;
    if(!S_ISDIR(st.st_mode)) {
        return r;
    }
    
    DIR * dir = opendir(path);
    if(dir == NULL) {
        perror("opendir()");
        exit(1);
    }

    struct dirent * cur;
    while((cur = readdir(dir)) != NULL) {
        // skip special directory
        if(strcmp(cur->d_name, ".") == 0 || strcmp(cur->d_name, "..") == 0) {
            continue;
        }
        size_t len0 = strlen(path);
        size_t len1 = strlen(cur->d_name);

        // new path = path + '/n' + sub + '\0'
        char * new_path = malloc(len0 + len1 + 2);
        strcpy(new_path, path);
        new_path[len0] = '/';
        strcpy(new_path + len0 + 1, cur->d_name);

        r += du(new_path);
        free(new_path);
    }
    
    closedir(dir);
    return r;
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print directory/file disk usage recursively\nUsage: %s filename/dirname\n", argv[0]);
        exit(1);
    }

    double d = (double) du(argv[1]);
    int unit = 0;
    while(d > 1000) {
        d = d / 1024.0;
        unit ++;
    }

    fprintf(stdout, "du = %.2f%c\n", d, units[unit]);
    return 0;
}
