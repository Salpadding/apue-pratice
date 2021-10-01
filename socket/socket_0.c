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
void server(const char* host, const char* iface)
{
    if(host == NULL)
        host = "0.0.0.0";
    // create a udp socket
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    int t = 1;

    // enable broadcast
    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &t, sizeof(t)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    // enable multicast
    int multicast = strncmp(host, "224", 3) == 0;
    if (multicast)
    {
        struct ip_mreqn mreq;
        inet_pton(AF_INET, host, &mreq.imr_multiaddr);
        inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);

        if(iface == NULL) {
            printf("provide iface by -i eth0\n");
            exit(1);
        }
        mreq.imr_ifindex = if_nametoindex(iface);

        printf("create mulitcast group %s on iface %s\n", host, iface);
        if (setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
            perror("setsockopt()");
            exit(1);
        }
    }

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

    addr.sin_addr.s_addr = 0;

    if (
        bind(sd, (void *)&addr, sizeof(addr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    // receive and print
    // recv 和 recvfrom 用于从套接字读取数据放到缓冲区
    // recvfrom 在收到数据的同时能确定对方的地址
    struct msg_st msg;
    struct sockaddr_in r_addr;
    socklen_t r_addr_len = sizeof(r_addr);
    char r_ip[IP_SIZE];
    while (1)
    {
        // try to receive a msg from socket
        recvfrom(sd, &msg, sizeof(msg), 0, (void *)&r_addr, &r_addr_len);

        // inet_ntop 把整数转成点分式
        inet_ntop(AF_INET, &r_addr.sin_addr, r_ip, INET_ADDRSTRLEN);
        printf("receive udp packet from %s:%d\n", r_ip, ntohs(r_addr.sin_port));
        printf("name = %s, math = %d, chinese = %d\n", msg.name, ntohl(msg.math), ntohl(msg.chinese));
    }

    close(sd);
    exit(0);
}

void client(const char *host, const char *iface)
{
    // create a udp socket to send packet
    // 如果要进行广播, 而且要打开广播功能
    // 使用 setsockopt 设置套接字属性
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    int t = 1;
    if (setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &t, sizeof(t)) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    int multicast = strncmp(host, "224", 3) == 0;
    // when host starts with 224, create multicast
    if (multicast)
    {
        struct ip_mreqn mreq;
        inet_pton(AF_INET, host, &mreq.imr_multiaddr);
        inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
        if(iface == NULL) {
            printf("provide iface by -i eth0\n");
            exit(1);
        }
        mreq.imr_ifindex = if_nametoindex(iface);

        printf("create mulitcast group %s on iface %s\n", host, iface);
        if (setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0) {
            perror("setsockopt()");
            exit(1);
        }
    }

    if (sd < 0)
    {
        perror("socket()");
        exit(1);
    }

    // bind 可以省略, 让操作系统自动分配

    // dest
    struct sockaddr_in r_addr;
    r_addr.sin_family = AF_INET;
    r_addr.sin_port = htons(RECV_PORT);

    if (inet_pton(AF_INET, host, &r_addr.sin_addr) != 1)
    {
        fprintf(stderr, "inet_pton()");
        exit(1);
    }

    struct msg_st msg;
    msg.chinese = htonl(100);
    msg.math = htonl(100);
    strcpy(msg.name, "sal");

    while (1)
    {
        if (sendto(sd, &msg, sizeof(msg), 0, (void *)&r_addr, sizeof(r_addr)) < 0)
        {
            perror("sendto()");
            exit(1);
        }
        sleep(1);
    }

    close(sd);
    exit(0);
}

int main(int argc, char **argv)
{
    if(argc == 1) {
        printf("Usage: \n");
        printf("run as udp server listen on 0.0.0.0:%d, %s -s \n", RECV_PORT, argv[0]);
        printf("run as udp client send to [hostname]:%d, %s -c -h hostname\n", RECV_PORT, argv[0]);
        printf("run as udp client broadcast to 255.255.255.255:%d, %s -c -h 255.255.255.255\n", RECV_PORT, argv[0]);
        printf("run as udp server listen to mulicast group 224.2.2.2:%d, %s -s -h 224.2.2.2 \n", RECV_PORT, argv[0]);
        printf("run as udp client mulicast to 224.2.2.2:%d, %s -c -h 224.2.2.2\n", RECV_PORT, argv[0]);
        exit(0);
    }
    int ch;
    char *host = NULL;
    char *i = NULL;
    int s = 0;
    int c = 0;
    while ((ch = getopt(argc, argv, "sch:i:")) >= 0)
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
        case 'i':
            i = malloc(strlen(optarg) + 1);
            strcpy(i, optarg);
            break;
        default:
            break;
        }
    }

    if (s)
        server(host, i);
    else
        client(host, i);

    if (host != NULL)
        free(host);
    exit(0);
}