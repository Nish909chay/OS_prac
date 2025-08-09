/*
Write a small C program showing how output redirection can be done by closing (1) and opening a file. 
*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    int fd = open("text.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  // 0644 - owner read/write, others read
    if (fd < 0) {
        perror("unable to open");
        return 1;
    }

    // fd(0) = standard input (stdin)
    // fd(1) = standard output (stdout)
    // fd(2)= standard error (stderr)
    // Close stdout (fd(1) - stdout)
    close(1);

    // Duplicate fd so that it uses fd 1 (stdout)
    // It chooses the lowest available fd number to assign to the duplicate. i.e  1
    if (dup(fd) != 1) {
        perror("dup");
        return 1;
    }
    // now fd(1) and fd(3) both point to text.txt
    close(fd); // original fd no longer needed

    // BUT as fd(1) is actually stdout, anything you printf will go to text.txt 
    printf("MnM\n");

    return 0;
}
