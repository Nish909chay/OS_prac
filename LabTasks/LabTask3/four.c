/*

Write a program that calls fork() twice, or thrice, and one/two of them inside some if-else. 
Then draw a diagram showing how the fork calls work. 
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork1 failed");
        return 1;
    }
    
    if (pid1 == 0) 
    {
        // Child process 1
        printf("Child 1: PID %d, Parent PID %d\n", getpid(), getppid());

        pid2 = fork();
        if (pid2 < 0) {
            perror("fork2 failed");
            return 1;
        }
        
        if (pid2 == 0) {
            // Child process 2 (child of Child 1)
            printf("Child 2: PID %d, Parent PID %d\n", getpid(), getppid());
        } else {
            printf("Child 1 after fork2: PID %d, Parent PID %d\n", getpid(), getppid());
        }
    } 
    else {
        // Parent process
        printf("Parent: PID %d, Child1 PID %d\n", getpid(), pid1);
    }

    return 0;
}
