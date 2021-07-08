#include "./anytimer.h"
#include <stdio.h>
#include <stdlib.h>

static void print_str(void* str) {
    puts(str);
}

int main() {
    at_id id1 = at_add_job(2, print_str, "hello world2");
    at_id id2 = at_add_job(5, print_str, "hello world5");    
    at_id id3 = at_add_job(7, print_str, "hello world7");    
    at_wait(id1);
    at_wait(id2);
    at_wait(id3);
}