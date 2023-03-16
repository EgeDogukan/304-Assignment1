#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    pid_t pid = fork();
    
    if(pid < 0) {
        printf("Fork failed!");
        exit(1);
    }
    else if(pid == 0) {
        printf("Parent PID: %d, Child PID: %d\n", getppid(), getpid());
        exit(0);
    }
    else {
        sleep(10);
        wait(NULL);
    }

    return 0;
}