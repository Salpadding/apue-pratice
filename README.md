# apue-pratice
apue pratices

标准IO vs 系统调用 Io

1. 可移植性好，适用于不同的 kernal
2. 效率高 cache 和 buffer 机制 合并系统调用

标准 IO 提供的函数

- fopen fclose fgetc fputc fgets fputs fread fwrite printf scanf 文件打开关闭，读写
- fseek ftell rewind 文件位指针操作
- fflush 缓冲



```c
char * ptr = "abc";
ptr[0] = 'x';
```

以上代码可能会产生段错误，如果编译器把 ptr 放在静态区, 第二条语句会尝试修改静态区的内存


```c
const char * ptr = "abc";
ptr[0] = 'x';
```

使用 const 声明后，编译器会在编译时提醒有对只读地址的内存修改，编译会不通过


```c
FILE* fopen(const char * path, const char * mode);
```

fopen 中使用 const 声明可以防止 path 指向的内存被修改
