// illustrate usage of setjmp function, which jump between functions

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf buf;

static void c(void) {
    printf("%s():Begin.\n", __FUNCTION__);
    printf("jump now\n");
    longjmp(buf, 1);
    printf("%s():End.\n", __FUNCTION__);
}


static void b(void) {
    printf("%s():Begin.\n", __FUNCTION__);
    printf("%s():Call c().\n", __FUNCTION__);
    c();
    printf("%s():c() returned.\n", __FUNCTION__);
    printf("%s():End.\n", __FUNCTION__);
}


static void a(void) {
    printf("%s():Begin.\n", __FUNCTION__);
    printf("%s():Call b().\n", __FUNCTION__);
    if (setjmp(buf) == 0) {
        b();
        printf("%s():b() returned.\n", __FUNCTION__);
        printf("%s():End.\n", __FUNCTION__);
    } else {
        printf("jumped from other function");
    }

}


int main() {
    a();
    return 0;
}