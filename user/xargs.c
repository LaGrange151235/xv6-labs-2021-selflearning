#include "kernel/types.h"
#include "user/user.h"

void run(char *program, char **args) {
    if (!fork()) {
        exec(program, args);
        exit(0);
    } else {
        return;
    }
}

int main(int argc, char *argv[]) {
    char buf[2048];
    char *p = buf, *last_p = buf;
    char *argsBuf[128];
    char **args = argsBuf;
    for (int i = 1; i < argc; ++i) {
        *args = argv[i];
        ++args;
    }
    char **pa = args;
    while (read(0, p, 1) != 0) { 
        /* 
         * read results of commands before from std pipe '|'
         * and put them as argvs for command after "xargs"
         */
        if (*p == ' ' || *p == '\n') {
            *p = '\0';
            *(pa++) = last_p;
            last_p = p+1;
            if (*p == '\n') {
                *pa = 0;
                run(argv[1], argsBuf); // when a line end with enter, execute the command
                pa = args;
            }
        }
        ++p;
    }
    if (pa != args) { // if the line is not ended with enter
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;
        run(argv[1], argsBuf);
    }
    while(wait(0) != -1)
        ;
    exit(0);
}