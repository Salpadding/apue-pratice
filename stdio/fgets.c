// when buf size = 5, read from abcd.txt need 2 loops
#include <stdio.h>
#include <stdlib.h>
#define BUF_SIZE 5

int main() 
{
    FILE* f = fopen("abcd.txt", "r");

    if(f == NULL) 
    {
        perror("fopen");
    }

    int n = 0;
    char buf[BUF_SIZE];
    while(1) 
    {
        n++;
        if(fgets(buf, BUF_SIZE, f) == NULL) {
            break;
        }
        fprintf(stdout, "%s\n", buf);
    }

    printf("n = %d\n", n);
}