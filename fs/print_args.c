// print command line arguments
// in zsh, *.c will expanded as ftype.c hole.c
#include <stdio.h>


int main(int argc, char** argv) {
    fprintf(stdout, "Your command line arguments is:\n");

    for(int i = 0; i < argc; i++) {
        fprintf(stdout, "%s\n", argv[i]);
    }

    return 0;
}
