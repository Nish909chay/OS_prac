#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if(pid == 0) {
        sleep(5);
        return 0;
    } else {
        while(1) {
            printf("Parent sleeping\n");
            sleep(1);
            break;
        }
        wait(NULL);
    }
    return 0;
}

