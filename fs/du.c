// print directory/file disk usage recursively
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <glob.h>

static const char * units = "bkmg";
static const double k = 1024.0;

int str_ends_with(const char * src, const char * suffix) {
    size_t len0 = strlen(src);
    size_t len1 = strlen(suffix);

    if(len0 < len1)
        return 0;
    
    const char * end0 = src + len0 - 1;
    const char * end1 = suffix + len1 - 1;

    for(size_t i = 0; i < len1; i++) {
       if (*(end0 - i) != *(end1 - i)) {
           return 0;
       }
    }

    return 1;
}

uint64_t du_by_glob(const char * path) {
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
    
    size_t len = strlen(path);

    // pattern = path + "/*" + '/0'
    char * pattern = malloc(len + 3);
    strcpy(pattern, path);
    strcpy(pattern + len, "/*");

    glob_t g;

    // when meet empty directory
    glob(pattern, 0, NULL, &g);
    free(pattern);

    // pattern = path + "/.*" + '/0'
    pattern = malloc(len + 4);
    strcpy(pattern, path);
    strcpy(pattern + len, "/.*");

    // when meet empty directory
    glob(pattern, GLOB_APPEND, NULL, &g);

    free(pattern);

    for(int i = 0; i < g.gl_pathc; i++) {
        // skip directory ends with /. or /..
        if(
            str_ends_with(g.gl_pathv[i], "/.") ||
            str_ends_with(g.gl_pathv[i], "/..")            
        ) continue;
        r += du_by_glob(g.gl_pathv[i]);
    }
    globfree(&g);
    return r;
}

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

    #ifdef DU_BY_GLOB
        printf("du by glob\n");
    #else
        printf("du\n");
    #endif    

    double d = (double) 
    #ifdef DU_BY_GLOB
        du_by_glob(argv[1]);
    #else
        du(argv[1]);    
    #endif

    int unit = 0;
    while(d > 1000) {
        d = d / 1024.0;
        unit ++;
    }

    fprintf(stdout, "du = %.2f%c\n", d, units[unit]);
    return 0;
}
