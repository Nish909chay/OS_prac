/*
Show that you can call execl(), execvp() and execlp() 
*/
#include<stdio.h>
#include<unistd.h>

int main() {
    // You must provide the full absolute or relative path for execl()
    // printf("Calling execl()\n");
    // execl("/bin/ls", "ls", "-l", NULL);      
    // perror("execl failed");

    // it takes a filename and not path like execl() 
    // and searches the directories listed in the PATH environment variable to find the executable
    // printf("Calling execlp()\n");
    // execlp("ls", "ls", "-l", NULL);
    // perror("execlp failed");

    // similar to execp(), it searches the path for file,
    // here the arguments are passed as array of str in argv[]
    printf("Calling execvp() \n");
    char *args[] = {"ls", "-l", "/desktop", NULL};
    execvp("ls", args);
    perror("execvp failed");

    return 0;
}