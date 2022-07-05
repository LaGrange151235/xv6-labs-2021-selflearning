#include "kernel/types.h"
#include "user/user.h"

int 
main(int argc, char *argv[]) {
    /* 
     * argc: number of parameters, including the name of program
     * argv: list of parameters, argv[0] is the name of program
     */
    if (argc < 2) {
        printf("need a parameter after 'sleep'\n");
        exit(1);
    }
    sleep(atoi(argv[1]));
    exit(0);
}