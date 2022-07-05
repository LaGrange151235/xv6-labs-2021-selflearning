#include "kernel/types.h"
#include "user/user.h"

void newProcess(int *pipe_left) {
    int pid = 0;
    int pipe_right[2];
    int p, n;
    read(pipe_left[0], &p, sizeof(p));
    if (!p)
        exit(0);
    pipe(pipe_right);
    pid = fork();
    if (pid) {  
        printf("prime %d\n", p);  
        close(pipe_right[0]); // no need to read from right for parent
        n = p;
        while (n) {
            read(pipe_left[0], &n, sizeof(n));
            if (n % p)
                write(pipe_right[1], &n, sizeof(n));
        }
        n = 0;
        write(pipe_right[1], &n, sizeof(n));
        wait(0);
    } else {
        close(pipe_left[0]); // no need to have access to this pipe for child
        close(pipe_right[1]); // no need to write to left for child
        newProcess(pipe_right);
    }
}

int main() {
    int pid;
    int pipe_right[2];
    int p;
    pipe(pipe_right);
    pid = fork();
    if (pid) {
        close(pipe_right[0]); // no need to read from right for parent
        for (p = 2; p <= 35; ++p) {
            write(pipe_right[1], &p, sizeof(p));
        }
        p = 0;
        write(pipe_right[1], &p, sizeof(p));
        wait(0);
    } else {
        close(pipe_right[1]); // no need to write to left for child
        newProcess(pipe_right);
    }
    exit(0);
}