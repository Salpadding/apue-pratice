# apue-pratice
apue pratices

## 标准 IO

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

提出问题: 如何安全地读取一行?

- getline: 使用前需要宏定义 #define _GNU_SOURCE 

```c
ssize_t getline(char ** lineptr, size_t *n, FILE * stream);
```

getline 函数有两个返回值，其中一个类型是 char * , 需要传一个 char ** 进去，另一个类型是 size_t, 需要传指针

临时文件: 使用完就删除, 创建时 不能冲突

tmpnam 这个函数是线程不安全的，而且不同的进程可能会获取到相同的 tmpnam

```c
char * tmpnam(char * s)
```

tmpfile，返回一个匿名文件，当 fclose 这个临时文件后，这个文件会被删除

```c
FILE * tmpfile(void);
```

## 系统调用 IO

文件描述符 file descriptor

FILE * 结构体猜测

1. cache 和 buffer 区
2. 文件描述符
3. 文件位指针

使用 open 打开文件后，会创建一个结构体，这个结构体包含了这个文件的 inode，文件位指针，这个结构体会存在于一个数组中，文件描述符就是这个结构体的数组下标
文件描述符为什么是一个整形数? 因为它是一个数组下标

文件描述符优先使用当前可用范围内最小的，文件描述符会引用一个隐藏的结构体，同一个隐藏的结构体可能会被多个描述符引用，当没有描述符引用该结构体时，结构体会被释放

文件 IO 操作:

open, close, read, write, lseek

```c
int open(const char * pathname, int flags);
int open(const char * pathname, int flags, int mode);
```

其中 flags 是位图，常用的选项

- O_CREAT 无则创建
- O_TRUNC 有则清空
- O_EXCL 必须打开新文件，必须和 O_CREATE 连用
- O_APPEND 追加写
- O_ASYNC 信号驱动 IO
- O_DIRECT 最小化 cache 作用
- O_LARGEFILE 大文件打开 或者设置 off_t
- O_NOATIME 不修改文件最后读时间
- O_NOFOLLOW 不解引用符号连接
- O_NONBLOCK 非阻塞 IO
- O_SYNC 同步，跟信号驱动没关系

"r" = O_RDONLY, "r+" = O_RDWR, 
"w" = O_WRONLY | O_TRUNC | O_CREAT
"w+" = O_RDWR | | O_TRUNC | O_CREAT 
"a" = O_WRONLY |  O_CREAT | O_APPEND
"a+" = O_RDWR | O_CREAT | O_APPEND

如果 flags 中包含 O_CREAT，要添加 mode 选项，指定文件的权限位，权限位会被 umask 掩盖掉权限

IO 的效率问题

原子操作

重定向(dup, dup2)

同步 sync, fsync, fdatasync

fcntl, ioctl 管家级的函数

/dev/fd/ 虚目录


- read 函数

```c
ssize_t read(int fd, void * buf, size_t count);
```

成功返回读取的字节数，遇到 EOF 返回 0, 失败返回 -1

- write 函数

```c
ssize_t write(int fd, const void * buf, size_t count);
```

- lseek 函数

和 fseek 和 ftell 的综合使用，先移动文件位指针到指定位置

```c
off_t lseek(int fd, off_t offset, int whence);
```

ftello 相当于 lseek(fd, 0, SEEK_CUR)

标准 IO 和 系统调用 IO 不可以混用，因为标准 IO 有缓冲区，当从标准 IO 读取一个字节时，FILE 结构体中的文件描述符可能已经读了 4096 个字节，写入时也是同理


strace 可以调式系统调用的

原子操作: 原子是指不可分割的最小单位，原子操作的作用是解决竞争和冲突，比如 tmpnam 和 tmpfile，其中 tmpnam 就是不原子的

dup 和 dup2

dup 的问题在于没有检查传入的文件描述符就是需要重定向的文件描述符，需要先关闭，不具备原子性，不支持多线程
dup2 会把 newfd 作为 oldfd 的副本，如果必要的话，会关闭 oldfd

dup2(newfd, oldfd)


/dev/fd 显示当前进程打开的文件描述符
## 文件系统

- 目录和文件

- 系统数据文件和信息
