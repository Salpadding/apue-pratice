/**
 * example for read unix style argument from command line
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// parse arguments from command line,
// opt -a argumentA -b, pass argumentA as value for -a flag, set -b flag as true

int main(int argc, char ** argv) {
    int ch;

    while((ch = getopt(argc, argv, "-a:b")) >= 0) {
        switch (ch)
        {
        case 'a':
            printf("-a value = %s\n", optarg);
            break;
        case 'b':
            printf("-b value = true\n");
            break;
        default:
            break;
        }
    }
    return 0;
}
