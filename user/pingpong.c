#include "kernel/types.h"
#include "user/user.h"

int main() {
    int pipe1[2], pipe2[2];
    pipe(pipe1); // from parent to child
    pipe(pipe2); // from child to parent
    int pid;
    pid = fork();
    if (pid) {
        // parent
        char byteBuf;
        write(pipe1[1], "a", 1);
        wait(0); // 0 means 'NULL'
        read(pipe2[0], &byteBuf, sizeof(byteBuf));
        printf("%d: received pong\n", getpid());
    } else {
        // child
        char byteBuf;
        read(pipe1[0], &byteBuf, sizeof(byteBuf));
        printf("%d: received ping\n", getpid());
        write(pipe2[1], "A", 1);
    }
    exit(0);
}