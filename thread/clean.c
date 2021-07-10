#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static void cleanup_func(void* str) {
    const char* s = str;
    puts(s);
}

static void* my_echo(void* _) {
    puts("hello pthread");

    // 在线程退出时进行清理操作
    // pthread_cleanup_push 和 pthread_cleanup_pop 必须成对出现
    pthread_cleanup_push(cleanup_func, "clean:1");
    pthread_cleanup_push(cleanup_func, "clean:2");
    pthread_cleanup_pop(1); // 1 表示调用
    pthread_cleanup_pop(1);

    pthread_exit(NULL);
}

int main() {
    pthread_t id;
    int r;

    puts("Begin!");
    r = pthread_create(&id, NULL, my_echo, NULL);
    if(r != 0) {
        fprintf(stderr, "pthread_create() failed: %s\n", strerror(r));
        exit(1);
    }
    r = pthread_join(id, NULL);
    if(r != 0) {
        fprintf(stderr, "pthread_join() failed: %s\n", strerror(r));
        exit(1);
    }    
    puts("End");
    exit(0);
}
