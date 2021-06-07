// print the stat of dst file

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// print stat into out
void fprint_stat(FILE * out,  struct stat * data) {
    // 包含该文件的设备号, 你打开的必须是个文件
    fprintf(out, "dev = %ld\n", data->st_dev);
    // inode
    fprintf(out, "inode = %ld\n", data->st_ino);
    // 权限信息
    fprintf(out, "mode = %d\n", data->st_mode);
    // 硬连接数
    fprintf(out, "hard links = %ld\n", data->st_nlink);
    // 所有者
    fprintf(out, "userid = %d\n", data->st_uid);
    // 所属的组
    fprintf(out, "groupid = %d\n", data->st_gid);    
    // 如果打开的是设备，等于打开的设备的设备号
    fprintf(out, "rdev = %ld\n", data->st_rdev);        
    // 文件大小
    fprintf(out, "total size = %ld\n", data->st_size);
    // block 包含的字节数量
    fprintf(out, "block size = %ld\n", data->st_blksize);    
    // 当前文件占了多少个 512 字节
    fprintf(out, "block count = %ld\n", data->st_blocks);        
    fprintf(out, "atime %ld\n", data->st_atim);
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "print stat of file\nUsage %s filename\n", argv[0]);
        exit(0);
    }

    struct stat st;

    if(stat(argv[1], &st) < 0) {
        perror("stat()\n");
        exit(0);
    }

    fprint_stat(stdout, &st);
    return 0;
}