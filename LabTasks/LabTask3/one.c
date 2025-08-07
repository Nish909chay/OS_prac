/*
    Show that exec() can fail, for at least 2 different reasons
    Show that you can call execl(), execvp() and execlp() 
*/
#include<stdio.h>
#include<unistd.h>
int main()
{
    execl("/np", "np", NULL);
    perror("not found");
}