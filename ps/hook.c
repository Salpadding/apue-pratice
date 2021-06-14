#include <stdio.h>
#include <stdlib.h>

static void hook() {
    puts("Hook!");
}

int main() {
    atexit(hook);
    puts("Begin!");
    puts("End!");
}