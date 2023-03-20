#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>

#define MAX_SEQ_LEN 1000


int main(int argc, char* argv[]) {

    if(argc != 3) {
        fprintf(stderr, "Usage: %s <number to search> <number of children>\n", argv[0]);
        return 1;
    }

    int x = atoi(argv[1]);
    int n = atoi(argv[2]);

    if(n <= 0 || n > MAX_SEQ_LEN) {
        fprintf(stderr, "Error: number of children must be between 1 and %d\n", MAX_SEQ_LEN);
        exit(1);
    }

    int seq_len = 0;
    int num;
    int seq_arr[MAX_SEQ_LEN];

    while(scanf("%d", &num) == 1 && seq_len < MAX_SEQ_LEN) {
        seq_arr[seq_len] = num;
        seq_len++;
    }

    int shmid = shmget(IPC_PRIVATE, sizeof(int) * seq_len, IPC_CREAT | 0666);   //creating the shared memory segment

    if(shmid < 0) {
        fprintf(stderr, "Error: failed to create shared memory segment\n");
        exit(1);
    }

    int *seq;
    seq = (int *)shmat(shmid, NULL, 0);                                         //attaching the shared memory segment

    if(seq == (int *)-1) {
        fprintf(stderr, "Error: failed to attach shared memory segment\n");
        exit(1);
    }

    for(int i = 0; i < seq_len; i++) {                                          //moving the sequence to the shared memory segment we have just recenlty attached
        seq[i] = seq_arr[i];
    }



    int seg_size = seq_len / n;
    int remainder = seq_len % n;
    int pid;

    for(int i = 0; i < n; i++) {
        pid = fork();

        if(pid < 0) {
            fprintf(stderr, "Error: failed to fork child process\n");
            exit(1);
        } else if(pid == 0) {
            // Child process
            int start = i * seg_size;
            int end = start + seg_size;
            if(i == n - 1) {
                end += remainder;
            } 

            for(int j = start; j < end; j++) {
                if(seq[j] == x) {
                    printf("Child %d status: Succes at index (1 - %d): %d    exiting...\n", i + 1, seq_len, j + 1);
                    exit(0);
                }
            }
            printf("Child %d status: Failed\n", i + 1);
            exit(1);
        }
    }

    int status;

    for(int i = 0; i < n; i++) {
        pid = wait(&status);
        
        if(WIFEXITED(status)) {
            if(WEXITSTATUS(status) == 0) {
                printf("\n\n---------------------\n");                          //a child found the number, so we kill other processes and exit
                for(int j = 0; j < n; j++) {
                    if(j != i) {
                        kill(pid + j - i, SIGTERM);
                        printf("Child %d has been killed\n", j + 1);
                    }
                }

                shmdt(seq);                                                     //detaching and destroying the shared memory segment we have created 
                shmctl(shmid, IPC_RMID, NULL);

                exit(0);
            }
        } else {
            fprintf(stderr, "Error: child process %d exited abnormally\n", pid);
        }
    }

    shmdt(seq);                                                                 //number not found, still detaching and destroying the segment
    shmctl(shmid, IPC_RMID, NULL);
    printf("Wanted number %d could not be found\n", x);
    
    exit(1);

}