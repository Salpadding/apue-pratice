// mycpy srcfile dstfile
// copy file from source to destination
// 1. open srcfile readonly r
// 2. open dstfile write only (truncate ) w

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

void try_close(FILE *fp) {
    if(fp == NULL)
        return;
    fclose(fp);
}

int main(int argc, char ** argv) {
    if(argc < 3) {
        fprintf(stderr, "copy from src file to dst file:\n%s srcfile dstfile\n", argv[0]);
        exit(0);
    }
    FILE * src = fopen(argv[1], "r");
    if(src == NULL) {
        perror("fopen()");
        exit(0);
    }

    FILE *dst = fopen(argv[2], "w");
    if(dst == NULL) {
        perror("fopen()");
        fclose(src);
        exit(0);
    }

    char c;

    while(TRUE) {
        c = fgetc(src);
        if(c == EOF)
            break;
        fputc(c, dst);
    }

    // 资源的关闭顺序和资源的打开顺序要相反，后期可以用钩子函数实现资源的清理
    fclose(dst);
    fclose(src);

    return 0;
}