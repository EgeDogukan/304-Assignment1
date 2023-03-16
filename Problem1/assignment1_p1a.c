#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>


int main(int argc, char const *argv[]) {

    if(argc < 2) {
        printf("No arguments!\n");
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int i = 0;
    int level = 1;
    int mainFlag = 0;
    
    pid_t mainPID = getpid();
    int x;
    for(x=1; x <= n ; x++){
        pid_t pid = fork();
    }

    if(getpid() == mainPID && mainFlag < 1) {
        printf("Main Process ID: %d, level: %d\n", getpid(), 0);
        mainFlag = 1;
    }
    if(getpid() != mainPID) {
        if((getpid() - mainPID > n)) {
            if((getpid() - mainPID) % n > 0 && n > 2) {
                level = floor((getpid() - mainPID) / n) + 1;
            }
            else if((getpid() - mainPID) % n > 0) {
                level = 2;
            }
            else if((getpid() - mainPID) % n == 0 && n > 2) {
                level = ceil((getpid() - mainPID) / n);              
            }
        }       
        printf("PROCESS ID: %d, Parent ID: %d, level: %d\n", getpid(), getppid(), level);
    }
    
    return 0;
}