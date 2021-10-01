// a simple http 1.0 client
// download a jpg file from http://127.0.0.1:8080/avatar.jpg
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

void download(const char *host, int port, const char *uri, const char *out_path)
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
    r_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &r_addr.sin_addr) != 1)
    {
        fprintf(stderr, "inet_pton()");
        exit(1);
    }

    // create a tcp connection
    if (connect(sd, (void *)&r_addr, sizeof(r_addr)) < 0)
    {
        perror("connect()");
        exit(1);
    }

    // write http method
    FILE *fp = fdopen(sd, "r+");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    fprintf(fp, "GET %s HTTP/1.0\r\n\r\n", uri);
    // 当前为全缓冲模式,需要刷新缓冲区
    fflush(fp);

    FILE *out = fopen(out_path, "w");

    if (out == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    // body is after \r\n\r\n
    int c = 0;
    int status = 0;
    int body = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == '\r' && status == 0)
        {
            status = 1;
        }
        if (c == '\n' && status == 1)
        {
            status = 2;
        }
        if (c == '\r' && status == 2)
        {
            status = 3;
        }
        if (c == '\n' && status == 3)
        {
            body = 1;
            continue;
        }
        if (c != '\r' && c != '\n')
            status = 0;

        if (body)
            fputc(c, out);
    }

    fclose(out);
    close(sd);
    exit(0);
}

char *load_opt()
{
    char *r = malloc(strlen(optarg) + 1);
    strcpy(r, optarg);
    return r;
}

void print_usage(char **argv)
{
    printf("Usage: \n");
    printf("download file from http://[host]:[port][uri] save to [file]\n%s -h [host] -p [port] -u [uri] -o [file]\n", argv[0]);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        print_usage(argv);
        exit(0);
    }
    int ch;
    char *host = NULL;
    char *port = NULL;
    char *uri = NULL;
    char *file = NULL;
    while ((ch = getopt(argc, argv, "h:p:u:o:")) >= 0)
    {
        switch (ch)
        {
        case 'h':
            host = load_opt();
            break;
        case 'p':
            port = load_opt();
            break;
        case 'u':
            uri = load_opt();
            break;
        case 'o':
            file = load_opt();
            break;
        default:
            break;
        }
    }

    if (host == NULL || port == NULL || uri == NULL || file == NULL)
    {
        print_usage(argv);
        exit(1);
    }

    download(host, atoi(port), uri, file);

    free(host);
    free(port);
    free(uri);
    free(file);
    exit(0);
}