#include <shadow.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>

char * get_salt(const char * s) {
    int cnt = 0;
    int i = 0;
    const char * cur = s;

    while(*cur != '\0') {
        if(*cur == '$') {
            cnt++;
        }
        if(cnt == 3) {
            char * r = malloc(i + 2);
            for(int j = 0; j < i+1; j++) {
                r[j] = s[j];
            }
            return r;
        }
        cur ++;
        i++;
    }

    return NULL;
}

int main(int argc, char ** argv) {
    if(argc < 2) {
        fprintf(stderr, "validate user password\nUsage: %s username\n", argv[0]);
        exit(1);
    }

    struct spwd * p = getspnam(argv[1]);

    if(p == NULL) {
        perror("getspnam()");
        exit(1);
    }

    const char * passwd = getpass("input your password\n");
    if(passwd == NULL) {
        perror("read password failed\n");
        exit(1);
    }

    // get salt from pwd
    char * salt = get_salt(p->sp_pwdp);
    char * crypted = crypt(passwd, salt);
    free(salt);

    if(strcmp(crypted, p->sp_pwdp) == 0) {
        printf("validate success\n");
    } else {
        fprintf(stderr, "validate failed\n");
    }
    return 0;
}

