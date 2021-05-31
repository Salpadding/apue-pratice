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

snprintf, gets, fgets 没有解决内存安全的问题，没有检查写越界
使用 scanf, fscanf, sscanf 要慎用 %s，因为你无法知道 scanf 输入字符串的长度

fseek, ftell 使用了 long 类型，而 long 类型没有标准定义，这就导致在使用 fseek 和 ftell 时不能对大文件作操作
fseeko 和 ftello 使用了 off_t 类型，可以在编译时进行宏替换

fseek: whence = SEEK_SET, SEEK_CUR, SEEK_END，
例如回到文件首 fseek(f, 0, SEEK_SET)

ftell 文件位置指针的当前位置

rewind 相当于 fseek(f, 0, SEEK_SET)，把文件位指针移到起始位置

空洞文件: 文件充斥着 '\0'，下载工具在下载文件时会创建一个空洞文件，目的是防止下载过程中报磁盘空间不足

fflush

```c
int main() {
    printf("before while");

    while(1);
}
```

以上代码执行时候终端不会打印，因为终端是行缓冲模式，打印结果还在缓冲区, fflush(null) 会刷新所有输出流

缓冲大部分情况下是好事，合并系统调用

行缓冲: 终端设备，遇到换行符或者缓冲区满了则刷新
全缓冲: 满了时候或者 
无缓冲: stderr，没有缓冲区，默认情况下不建议修改缓冲模式，如果需要，可以用 setvbuf 修改缓冲模式