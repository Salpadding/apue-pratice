// share same port between client and server
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
#include <pthread.h>

#define BIND_PORT 7077
#define MESSAGE "hello socket\n"
#define IP_SIZE 32

struct server_st 
{
    int sd;
    struct sockaddr_in addr;
};

static struct server_st server;

void* try_echo(void* args) 
{
    struct sockaddr_in s_addr;
    memcpy(&s_addr, &server.addr, sizeof(s_addr));
    inet_pton(AF_INET, "127.0.0.1", &s_addr.sin_addr);

    while(1)
    {
        sendto(server.sd, MESSAGE, strlen(MESSAGE), 0, (void*) &s_addr, sizeof(s_addr));
        sleep(1);
    }
}

int main()
{
    server.sd = socket(AF_INET, SOCK_DGRAM, 0);

    if(server.sd < 0) {
        perror("socket()");
        exit(1);
    }

    server.addr.sin_family = AF_INET;
    server.addr.sin_port = htons(BIND_PORT);

    // inet_pton 可以把点分式转成整形数
    if (inet_pton(AF_INET, "0.0.0.0", &server.addr.sin_addr) != 1)
    {
        fprintf(stderr, "inet_pton()");
        exit(1);
    }


    // bind and listen
    if (
        bind(server.sd, (void *)&server.addr, sizeof(server.addr)) < 0)
    {
        perror("bind()");
        exit(1);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, try_echo, NULL);

    // receive and print
    // recv 和 recvfrom 用于从套接字读取数据放到缓冲区
    // recvfrom 在收到数据的同时能确定对方的地址
    char* buf = malloc(strlen(MESSAGE) + 1);
    struct sockaddr_in r_addr;
    socklen_t r_addr_len = sizeof(r_addr);
    char r_ip[IP_SIZE];
    while (1)
    {
        // try to receive a msg from socket
        recvfrom(server.sd, buf, strlen(MESSAGE), 0, (void *)&r_addr, &r_addr_len);

        // inet_ntop 把整数转成点分式
        inet_ntop(AF_INET, &r_addr.sin_addr, r_ip, INET_ADDRSTRLEN);
        printf("receive udp packet %s from %s:%d\n", buf, r_ip, ntohs(r_addr.sin_port));
    }

    free(buf);
    close(server.sd);
}