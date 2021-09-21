// mycpy1 srcfile dstfile
// copy file from source to destination
// 1. open srcfile readonly r
// 2. open dstfile write only (truncate ) w

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 32

size_t safe_fread(FILE * fd, void * buf, size_t count) {
    return fread(buf, 1, count, fd);
}

size_t safe_fwrite(FILE * fd, const void * buf, size_t count) {
    return fwrite(buf, 1, count, fd);
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

    char buf[BUF_SIZE];

    while(TRUE) {
        size_t rd = safe_fread(src, buf, BUF_SIZE);
        if(rd <= 0)
            break;
        safe_fwrite(dst, buf, rd);
    }

    // 资源的关闭顺序和资源的打开顺序要相反，后期可以用钩子函数实现资源的清理
    fclose(dst);
    fclose(src);

    return 0;
}