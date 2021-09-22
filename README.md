# apue-pratice

apue pratices

apue 分成如下几大块:

1. I/O 第3, 5, 14 章
2. 文件系统 第4, 6, 7 章
3. 并发 第10, 11章
4. IPC 进程间通信 第8, 13, 15, 16 章

## 标准 IO

标准IO vs 系统调用IO(文件IO) 

标准IO:

1. 可移植性好，适用于不同的 kernal
2. 效率高 cache 和 buffer 机制 合并系统调用

标准 IO 提供的函数

- fopen fclose 打开/关闭
- fgetc fputc fgets fputs 字符串读写
- fread fwrite 二进制读写 
- printf族 scanf族 
- fseek ftell rewind 文件位指针操作
- fflush 强制刷新缓冲区

### const 关键字

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

fopen 中使用 const 声明可以防止 path 指向的内存被修改, 具体见 ```man fopen```

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

fputs 用于输出 c 字符串，c 字符串结尾会有一个 '\0' 

fread 用于二进制流的读取
fwrite 用于二进制流的写入
fread 和 fread 没有验证边界，读写过程中不可以出现错误

两种读法
a->数据量足够, 1 和 2 的返回相同
b->只有五个字节, 1 和 2 的 返回相同，而且难以预知行为
1. fread（buf, 1, 10, fp)
2. fread(buf, 10, 1, fp)

建议 fread 和 fwrite 都采用单字节对象操作

sprintf, gets, fgets 没有解决内存安全的问题，没有检查写越界
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

重定向标准输出到文件的实现方式:

1. close(1), dup(target_fd)
2. dup2(target_fd, i) 

dup 的问题在于没有检查传入的文件描述符就是需要重定向的文件描述符，需要先关闭，不具备原子性，不支持多线程
dup2 会把 newfd 作为 oldfd 的副本，如果必要的话，会关闭 oldfd

dup2(newfd, oldfd)


/dev/fd 显示当前进程打开的文件描述符
## 文件系统


- 系统数据文件和信息

stat包含的内容:

1. 设备 id
2. inode
3. 权限(文件类型, u+s, g+s, 粘住位)
4. 文件大小
5. 占用的扇区数量
6. number of hard links
7. atime, mtime, ctime

以上其实是 inode 中包含的内容, 可以注意到 inode 中不包含文件名, 因为文件名保存在目录文件中, 目录文件保存了文件名到 inode 的映射, 或者是子目录

在文件属性中, st_size 和  st_blocks * 512 没有必然联系，对于普通文件，
st_blocks * 512 会大于等于 st_size，对于空洞文件 st_size 会远大于 st_blocks * 512

st_blocks * 512 是文件实际占用的磁盘大小, st_size 仅仅是一个标记，用于记录文件的大小

st_mode(16bit) 记录了

1. 文件类型(3bit) dcblsp-

d = directory 目录 , c = character 字符设备, b = block 块设备, l = symbolic link 符号连接, s = socket, p = pipe 管道, - 常规文件

2. u+s g+s 粘住位 (3bit)

3. 文件的权限位 (9bit)

- 改变文件的权限信息

chmod, fchmod

4. 粘住位

可执行的文件如果设置了粘住位，那么下次装载的时候会比较块，现在常用在目录上，如果对一个目录设置了粘住位，则只有对该目录具有写许可权的用户并且满足下列条件之一，才能删除或更名该目录下的文件：1.拥有此文件 2.拥有此目录 3.是超级用户。

- hardlink

相当于创建一个相同 inode 的目录项, 同时增加 inode 的硬链接数

- symbolic link

是一个新的文件，可以有不同的 inode 等文件属性，不占磁盘空间，所有信息都存在 inode 中

```python
# 在一个 4k 数据块大小的 ext2 文件系统中，文件的最大大小
# 12 个数据块指针 + 1 个一级间接指针 + 1个 2 级间接指针 + 1个3级间接指针 
(12 + 256 + 256 * 256 + 256 * 256 * 256) * 4096
```

和硬链接有关的函数:

- link 不能跨分区建立硬连接，不能为目录创建硬连接
- unlink 可以用于创建匿名文件，不能用于删除空目录

和工作路径有关的系统调用

- chdir 常用于创建守护进程
- fchdir
- getcwd

5. glob 函数，通过模式寻找路径，需要用 globfree 释放

glob 的 pattern 

- /etc 目录下所有的文件，不包含隐藏文件 /etc/*
- /etc 目录下所有的隐藏文件, /etc/.*
- 子目录下的隐藏文件包含 /. 和 /.., 分别指向当前目录和上一级目录

6. 目录流操作

见 ```fs/readdir.c```

- opendir 打开目录流
- readdir_r 迭代目录流

7. 系统数据文件和信息

1. /etc/passwd

username:*:uid:gid:comment:home:bash

getpwuid, getpwnam 分别根据 uid 和 username 获取用户信息，返回的指针指向的内存位于静态区


2. /etc/group

groupname:*:groupdid:

对应的函数有 getgrgid, getgrnam

3. /etc/shadow

记录了用户密码的哈希值
通过 getspnam, getpass, get_salt, crypt 可以校验用户输入的密码

```fs/validate_passwd.c```

4. 时间戳

各种时间的转换可以参考 apue 6.10

- time 函数，从内核中取时间戳

```c
#include <time.h>
time_t time(time_t*);
```

- localtime 和 gmtime 函数, time_t -> struct tm*, 整数转结构体

```c
#include <time.h>
struct tm *localtime(const time_t *timep);
struct tm *gmtime(const time_t *timep);
```

- mktime struct tm* -> time_t, 结构体转整数

mktime 会把时间结构体格式化, 适合用于对日期做加减操作

```c
time_t mktime(struct tm *tm);
```

- strftime struct tm* -> char *, 

```c
#include <time.h>
size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
```            

## 进程环境

- 进程的终止方式

正常终止:

1. main 函数 return 0
2. exit() _exit() _Exit()
3. 最后一个线程从其启动例程返回
4. 最后一个线程调用了 pthread_exit

main 函数 return 0 或者调用 exit()，会调用 atexit 注册的钩子函数，钩子函数会按照注册顺序的逆序执行，可以利用钩子函数关闭打开的资源

异常终止:

1. 调用 abort 函数，发送 SIG_ABORT 信号给当前进程，同时生成 core dump
2. 接到一个信号并终止
3. 最后一个线程对其取消请求作出响应


- 命令行参数解析

1. getopt 函数

```c
int getopt(int argc, char * const argv[], const char *optstring);
```

此函数用于短格式命令行参数解析, 如果需要一个字符串类型的参数, 可以使用 "a:"


```c
int getopt_long(int argc, char * const *argv, const char *optstring, const struct option *longopts, int *longindex);
```

getopt_long 同时支持长格式和短格式的命令行传参

- 环境变量

获取环境变量需要声明 ```extern char ** argv```

对环境变量进行读写可以调用函数


```c
char * getenv(const char *name);

int setenv(const char *name, const char *value, int overwrite);

int putenv(char *string);

int unsetenv(const char *name);
```


- 函数间跳转

```c
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
```

setjmp 函数的特点是调用一次返回两次, jmp_buf 类型的值需要定义为全局变量，因为要在多处使用, setjmp 和 longjmp 的使用可以参考 ps/setjmp.c

注意 jmp_buf 必须声明为静态变量

进程空间具体见 apue 7.7

- 资源获取

```c
#include <sys/resource.h>

// 获取资源限制
int getrlimit(int resource, struct rlimit *rlp);

// 设置资源限制，需要 root 权限
int setrlimit(int resource, const struct rlimit *rlp);
```

rlp 包含了 soft limit 和 hard limit, 所有用户都可以修改 soft limit, 但是 soft limit 不能大于 hard limit, 只有 super user 可以修改 hard limit
resource 是宏，例如 RLIMIT_CORE

## 进程

进程的产生、消亡，父子进程的关系

进程标识符 pid, ```getpid, getppid```

进程的产生 fork, vfork

fork 执行一次，返回两次，复制当前进程，包括当前进程的执行位置
fork 父子进程的区别: fork 的返回值不同， pid 和 ppid 不同，未决信号和文件锁不继承，资源利用量归 0
fork 之前应该刷新缓冲区, 防止缓冲区被复制到子进程
fork 不会继承未决信号和文件锁

fork:

1. 成功, 父进程返回子进程的 pid, 子进程返回 0
2. 失败, 父进程返回 -1

进程可以看作一种资源，谁创建就需要谁释放，父进程需要 reap 子进程，如果父进程没有及时 reap 子进程，而且父进程没有退出，子进程会变成 zombie 进程，父进程退出后，子进程会变成孤儿进程，被init进程接管

现在操作系统的 fork 都使用了写时复制技术，父子进程谁修改内存，谁就拷贝, 所以 vfork 很少使用了, vfork 后的子进程只能调用 _exit 或者 exec 函数族中的函数，调用其他函数都属于未定义行为

init 进程是所有进程的祖先进程, pid 是 1

进程的消亡及释放资源

reap 的本质是等待进程状态发生变化

- wait() 

```c
pid_t wait(int *wstatus);
```


把 reap 的返回值放到 wstatus 指向的位置，如果 status 是空指针,会忽略 wstatus

WIFEXITED , 检查进程正常终止
WEXITSTATUS, 进程退出的状态码，前提是子进程正常终止
WIFSIGNALED, 子进程是被信号终止的
WTERMSIG, 返回子进程被打断的信号
WCOREDUMP, 是否产生了 core dump 文件


- waitpid()

根据 pid reap 子进程，指定 WNOHANG 选项可以非阻塞式的 reap

```c
pid_t waitpid(pid_t pid, int *wstatus, int options);
```

如果 pid < 0，会 reap any child process whose process group id is equal to the absolute value of pid
如果 pid = -1, 会 reap 所有的子进程
如果 pid = 0, 会 reap any child process whose process group id is equal to that fo the calling process
如果 pid > 0, 会 reap child process whose process id is equal to the value of pid


- exec 函数族

exec 函数族的功能是把当前的进程 image 替换成新的 image, 常见的用法:

- execl, execlp, execle 的参数必须用 NULL 结尾

具体见 ps/exec_test.c

用户权限及组权限

守护进程

ps axm
ps axf
ps ax -L

pid 顺序产生和文件描述符不一样

getpid
getppid

exec 之前也要调用 fflush 刷新缓冲流

```argv[0]``` 的意义: argv[0] 会显示在 ps 命令的进程树中

### 用户权限和组权限

- 普通用户不可以 cat /etc/shadow 但可以用 passwd 改自己的登录口令

real uid, effective uid, 鉴权时候看的是 effective uid
passwd 有 u+s 位，其他人调用 passwd effective uid 时会切换到 passwd 拥有者的身份

g+s 同理, 其他人调用有 g+s 权限时时 effective gid 会切换到文件所在组 gid

## 进程组、会话和作业控制

1. 控制终端, 会话, 会话首进程
2. 进程组, 前台进程组, 后台进程组

概念:

1. 会话创建时候没有控制终端, 控制终端是会话首进程打开的
2. 会话首进程打开控制终端后, 会成为该终端的控制进程
3. 终端断开后, 内核会向控制进程发送 SIGHUB 信号
4. 前台进程组是唯一能读写控制终端的进程组
5. 控制进程通常是 shell, shell 会在收到 SIGHUB 信号后转发给 shell 创建的任务并退出

- get, set 进程组 id

set 进程组可以视为把进程移动到其他进程组

```c
#include <unistd.h>
pid_t getpgrp(void);

# 当 pgid 为 0 时, 创建新的进程组, 新的进程组的 id 为 调用进程的 pid
int setpgid(pid_t pid, pid_t pgid);
```

- get, set 会话 id

setsid 会创建一个新的会话, 调用进程会成为新会话的首进程, 调用的进程的 pid 会成为新会话的 id

```c
#define _XOPEN_SOURCE 500

#include <unistd.h>
pid_t getsid(pid_t pid);
pid_t setsid(void);
```

### 解释器文件

解释器文件就是脚本文件, 内容类似下文

```sh
#!/usr/bin/env bash
echo -e 'hello world\n'
```

### acct

进程会计函数, free bsd 方言, 不可移植

类似的还有 python 脚本和 nodejs 脚本, 带 x 权限的脚本文件可以被执行

### 守护进程

守护进程一定是:

1. 会话的 leader, pid = sid, 
2. 进程组的 leader
3. 脱离控制终端, 使用 ps 查看, tty 那一列是 ?

创建守护进程的步骤:

1. 使用 fork 创建一个子进程
2. 调用 setsid 使子进程脱离控制终端, 成为新会话和新进程组的 leader
3. 切换工作目录到根目录, 重定向 0,1,2 到 /dev/null

### 系统日志

系统日志位于 /var/log 目录下，主日志是 /var/log/messages, 所有守护进程应该通过 syslogd 服务写系统日志

系统日志相关的函数:

```c
#include <syslog.h>

void openlog(const char* ident, int option, int facility);
void syslog(int priority, const char* format, ...);
void closelog(void);
```

### 单实例守护进程

锁文件: /var/run/daemon_name.pid
守护进程开机启动脚本: /etc/rc.d/rc.local, 不同 linux 发行版的配置方式不同


## 信号

1. 信号是软件中断
2. kill -l 可以查看所有信号，其中[1, 31] 是标准信号，[32, 64]以后是实时信号
3. ctrl+c 就是给当前进程发一个 SIGINT 信号, signal 函数可以让程序员自己定义信号的处理函数, 也可以使用 SIG_IGN 忽略信号,或者 SIG_DFL 恢复默认
4. 信号会打断阻塞的系统调用，也就是说当调用 open, write, read 时 errno 可能会是被信号打断了，是一个假错误，如果进程被 sleep 阻塞，也可能提前结束阻塞
5. 信号的行为不可靠，标准信号有可能会丢失，信号处理函数在执行时，如果同一个信号又收到了，当前处理函数的执行现场可能会被丢弃，信号处理函数的触发和内核的进程调度有关，内核的进程调度是不可靠的，标准信号的响应没有严格的顺序，标准信号对应了一个 pending 位图，所以多个相同的标准信号可能只会被响应一次。
6. 可重入函数，第一次调用还没有结束，就发生了第二次调用，第一次调用对第二次调用的结果没有产生影响，所有的系统调用都是可重入的，一部分库函数也是可重入的，比如 rand_r，不可重入的函数不可以用于信号处理函数

理解信号的关键在于 kernal 态转换到 user 态的时间点

- signal 函数

signal 函数接受一个信号的值, 和一个函数指针

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void int_handler(int sign) {
    printf("SIG_INT!\n");
    fflush(NULL);
}

int main() {
    // ignore SIG_INT which send by ctrl+c
    signal(SIGINT, SIG_IGN);

    signal(SIGINT, int_handler);
    for(int i = 0; i < 10; i++) {
        write(1, "*", 1);
        sleep(1);
    }
    return 0;
}
```

- kill 函数

```c
#include <signal.h>
int kill(pid_t pid, int sig);
```

kill 中的 pid, 如果 pid > 0, 信号发给指定进程
pid = 0 时，信号会被发送给当前进程同组的所有进程
pid = -1 时，发送给当前进程有权限发信号的所有进程
pid < -1 时，发送给和 pid 的绝对值相等的进程组的所有进程

kill 的 sig = 0, 可以被用于检测某个进程存在，同时也要检查 errno

- raise 函数

```c
#include <signal.h>
int raise(int sig);
```

给当前进程发送一个信号，相当于 raise(getpid(), sig) 或者说 pthread_kill(pthread_self(), sig)

- alarm 函数

以秒为单位，在一定时间后，发送 SIG_ALARM 信号给自己

```c
unsigned int alarm(unsigned int seconds);
```

alarm 多个不能同时使用

- pause 函数

```c
#include <unistd.h>
int pause(void);
```

进入阻塞状态，等待信号打断阻塞

程序中尽量不要使用 sleep 函数，因为有些平台上 sleep 函数是用 alarm + pause 实现的，如果你定义了 SIG_ALRM 的行为是程序退出，那么 sleep 可能会导致程序退出

- volatie 关键字

防止编译器过度优化，例如某些循环条件的变量，编译器可能认为循环条件永久成立，把循环优化成了死循环

- 信号屏蔽字

信号屏蔽字可以决定信号什么时候被响应

- sigpending 

这个函数的问题在于，这是一个系统调用，这个函数返回的时候，信号集已经被处理过了，而且这是没有和信号屏蔽字按位与之前的信号集

不能从信号处理函数中随意的往外跳，例如 setjmp, longjmp，因为信号响应函数调用结束后，内核要把信号屏蔽字解除阻塞，如果在信号处理函数中 longjmp, 可能以后再也收不到这个信号
如果要跳的话，需要调用 sigsetjmp 和 siglongjmp

- signal 面对的问题

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile FILE* src = NULL;
static volatile FILE* dst = NULL:

void on_exit(int sig) {
    if(src != NULL) {
        fclose(src);
        src = NULL;
    }
    if(dst != NULL) {
        fclose(dst);
        dst = NULL;
    }
}

int main() {
    signal(SIGINT, on_exit);
    signal(SIGQUIT, on_exit);
}
```

在这里 on_exit 有重入的风险，例如进程收到 SIGINT 信号后，假如在调用 fclose 后，在 src = NULL 之前，被 SIGQUIT 打断，这时候可能会造成 src 被 close 两次

sigaction 可以指定在信号处理函数执行时需要block的信号，以防止重入现象的发生

另一个问题，signal 不能知道信号的来源，信号的来源可能是来自其他进程，流控算法不应该响应其他进程发送的 ALRM 信号, sigaction 可以通过检查 siginfo_t 类型的 si_code 判断信号是否来自内核q

```c
static void my_action(int sig, siginfo_t* info, void * _) {
    // 
    if(info->si_code == SI_KERNEL) {
        exit(0);
    }
}

sa.sa_flags = SA_SIGINTO;
```

- 令牌桶算法

每隔一秒钟生成 CPS 个 token
见 ```signal/mytbf.h```

- any timer

使用 alarm 可以实现计划任务
见 ```signal/anytimer.h```

- 实时信号

标准信号的不足：标准信号可能会丢

标准信号的响应顺序是未定义的，如果一个进程同时收到标准信号和实时信号，优先响应标准信号
SIGUSR1, SIGUSR2，以及实时信号是没有默认行为的

实时信号不会丢失，实时信号会排队，实时信号在调用处理函数时，收到的实时信号会进入队列

## 线程

1. 线程的概念

线程是一个正在运行的函数

2. 线程的创建(pthread_create)



3. 线程的终止(pthread_exit) 、收尸和栈的清理


4. 线程同步

- 互斥量


5. 线程属性、线程同步的属性

6. 线程和信号的关系、线程与 fork 的关系

7. 可重入