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

mode:

1. r 只读，文件必须存在
2. r+ 读写，文件必须存在
3. w 写，有则清空无则创建
4. w+ 读写，无则创建
5. a 追加写，无则创建
6. a+ 可以读，可以追加写，文件位指针的位置取决你是先读还是先写

对于指针类型，必须明确指针的内存位置（静态区，栈，堆）

文件描述符上限是 1024, 0=标准输入 1=标准输出 2=错误输出

文件默认权限 = 0777 & (~umask)



fgets 用于读字符串，函数结束的可能

1. 读了 size - 1 个字节, 有一个字节是留给 '\0' 的
2. 读到了 \n

fputs 用于输出 c 字符串，c 字符串会有一个 '\0' 

fread 用于二进制流的读取
fwrite 用于二进制流的写入
fread 和 fread 没有验证边界，读写过程中不可以出现错误

两种读法
a->数据量足够, 1 和 2 的返回相同
b->只有五个字节, 1 和 2 的 返回相同，而且难以预知行为
1. fread（buf, 1, 10, fp)
2. fread(buf, 10, 1, fp)

建议 fread 和 fwrite 都采用单字节对象操作