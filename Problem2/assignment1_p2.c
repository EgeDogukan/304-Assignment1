#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>


int main(int argc, char *argv[]) {
    
    if(argc < 3) {
        printf("Not enough arguments!\n");
        exit(1);
    }

    int n = atoi(argv[1]);
    char **command = &argv[2];
    pid_t pid;
    int pipefd[100][2];
    struct timeval start;
    struct timeval end;
    int devnull = open("/dev/null", O_WRONLY);
    int status;

    for(int i = 0; i < n; i++) {                //creating pipes for each n children
        if(pipe(pipefd[i]) == -1) {
            fprintf(stderr, "Pipe failed");
            return 1;
        }
    }

    for(int i = 0; i < n; i++) {                //forking child processes for n
        pid = fork();
        if(pid < 0) {
            fprintf(stderr, "Fork failed");
            return 1;
        }
        else if(pid == 0) {                     //child process
            close(pipefd[i][0]);                //closing unused read end of the child
            gettimeofday(&start, NULL);
            pid = fork();
            if(pid < 0) {
                fprintf(stderr, "Fork failed");
                return 1;
            }
            else if(pid == 0) {                 //grandchild process
                close(pipefd[i][1]);            //closing out wrtite ends
                dup2(devnull, STDOUT_FILENO);   //forwarding streams to /dev/null
                dup2(devnull, STDERR_FILENO);
                if(execvp(command[0], command) < 0) {   //executing command
                    fprintf(stderr, "Execvp failed");
                    return 1;
                }
            }
            else {
                waitpid(pid, &status, 0);        //waiting for specific process to finish
                gettimeofday(&end, NULL);
                double timeElapsed = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0; //calculating elapsed time and sending it
                write(pipefd[i][1], &timeElapsed, sizeof(double));
                close(pipefd[i][1]);
                return 0;
            }
        }
        else {
            close(pipefd[i][1]);                //Back at parent closing unused write end
        }
    }

    double results[100];
    for(int i = 0; i < n; i++) {
        read(pipefd[i][0], &results[i], sizeof(double));
        close(pipefd[i][0]);
    }

    double sum = 0;                             //finally starting to print the results
    double max = results[0];
    double min = results[0];

    for(int i = 0; i < n; i++) {
        printf("Child %d Executed in %.2f millis\n", i + 1, results[i]);
        sum += results[i];
        if(results[i] > max) {
            max = results[i];
        }
        if(results[i] < min) {
            min = results[i];
        }
    }

    printf("\n\n");
    printf("Max: %.2f millis\n", max);
    printf("Min: %.2f millis\n", min);
    printf("Average: %.2f millis\n", sum / n);

    return 0;

}