#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/if.h>

#define NAME_SIZE 16
#define RECV_PORT 7077
#define IP_SIZE 32
// define message format
struct msg_st
{
    char name[NAME_SIZE];
    u_int32_t math;
    u_int32_t chinese;
} __attribute__((packed));

// run as server
void server()
{
    // create a tcp socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // bind 的时候传的类型, 要根据不同的协议传不同类型的结构体指针
    // 这里是 ip 协议, 所以去 man ip
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(RECV_PORT);

    // inet_pton 可以把点分式转成整形数
    if (inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr) != 1)
    {
        fprintf(stderr, "inet_pton()");
        exit(1);
    }

    if (
        bind(sd, (void *)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    // listen
    if (listen(sd, 0xFFFFFFFF) < 0)
    {
        perror("listen()");
        exit(1);
    }

    // waiting for first connection
    struct sockaddr_in r_addr;
    socklen_t r_addr_len = sizeof(r_addr);
    int fd = accept(sd, (void *)&r_addr, &r_addr_len);
    char r_ip[IP_SIZE];
    inet_ntop(AF_INET, &r_addr.sin_addr, r_ip, INET_ADDRSTRLEN);
    printf("Client: ip = %s, port = %d\n", r_ip, ntohs(r_addr.sin_port));

    if (fd < 0)
    {
        perror("accept");
        exit(1);
    }

    // receive and print
    // recv 和 recvfrom 用于从套接字读取数据放到缓冲区
    // recvfrom 在收到数据的同时能确定对方的地址
    struct msg_st msg;
    while (1)
    {
        // try to receive a msg from socket
        int r = read(fd, &msg, sizeof(msg));
        printf("receive msg from client, name = %s math = %d, chinese = %d\n", msg.name, ntohl(msg.math), ntohl(msg.chinese));
        if (r == 0)
        {
            printf("client closed\n");
            break;
        }
    }

    close(fd);
    close(sd);
    exit(0);
}

void client(const char *host)
{
    // create a tcp socket to send packet
    int sd = socket(AF_INET, SOCK_STREAM, 0);

    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // bind 可以省略, 让操作系统自动分配

    // connect

    // dest
    struct sockaddr_in r_addr;
    r_addr.sin_family = AF_INET;
    r_addr.sin_port = htons(RECV_PORT);

    if (inet_pton(AF_INET, host, &r_addr.sin_addr) != 1)
    {
        fprintf(stderr, "inet_pton()");
        exit(1);
    }

    if (connect(sd, (void *)&r_addr, sizeof(r_addr)) < 0)
    {
        perror("connect()");
        exit(1);
    }

    struct msg_st msg;
    msg.chinese = htonl(100);
    msg.math = htonl(100);
    strcpy(msg.name, "sal");

    while (1)
    {
        printf("try to send msg to server\n");
        if (write(sd, &msg, sizeof(msg)) < 0)
        {
            perror("write()");
            exit(1);
        }
        sleep(1);
    }

    close(sd);
    exit(0);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("Usage: \n");
        printf("run as tcp server listen on 0.0.0.0:%d, %s -s \n", RECV_PORT, argv[0]);
        printf("run as tcp client send to [hostname]:%d, %s -c -h hostname\n", RECV_PORT, argv[0]);
        exit(0);
    }
    int ch;
    char *host = NULL;
    int s = 0;
    int c = 0;
    while ((ch = getopt(argc, argv, "sch:")) >= 0)
    {
        switch (ch)
        {
        case 's':
            printf("run as server\n");
            s = 1;
            break;
        case 'c':
            printf("run as client\n");
            c = 1;
            break;
        case 'h':
            host = malloc(strlen(optarg) + 1);
            strcpy(host, optarg);
            break;
        default:
            break;
        }
    }

    if (s)
        server();
    else
        client(host);

    if (host != NULL)
        free(host);
    exit(0);
}