/**
 * example 
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char ** argv) {
    struct option  long_options[] = {
        {"argument",  required_argument, NULL, 0}, 
        {"bool", no_argument, NULL, 0},
        // terminator
        {0, 0, 0, 0}
    };

    while(1) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "a:b", long_options, &option_index);

        if(c < 0)
            break;
        
        switch(c) {
            // long option found
            case 0: {
                struct option *o = long_options + option_index;
                if(o->has_arg)
                    printf("option %s = %s\n", o->name, optarg);
                else
                    printf("option %s set\n", o->name);
                break;
            }
            // short option found
            case 'a':
                printf("option a = %s\n", optarg);
                break;    
            case 'b':
                printf("option b set\n");
                break;           
            default :
                break;           
        }
    }
}
