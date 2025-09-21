#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pid_t pid1, pid2;

    // Create the pipe
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // First child -> "ls"
    pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0) {
        // Child 1: write to pipe
        dup2(fd[1], STDOUT_FILENO);  // redirect stdout to pipe write end
        close(fd[0]); // not used
        close(fd[1]); // already duplicated

        char *args[] = {"ls", NULL};
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }

    // Second child -> "wc -l"
    pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        exit(1);
    }

    if (pid2 == 0) {
        // Child 2: read from pipe
        dup2(fd[0], STDIN_FILENO);  // redirect stdin to pipe read end
        close(fd[1]); // not used
        close(fd[0]); // already duplicated

        char *args[] = {"wc", "-l", NULL};
        execvp(args[0], args);
        perror("execvp");
        exit(1);
    }

    // Parent process: close both ends and wait
    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}

