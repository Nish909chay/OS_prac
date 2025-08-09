/*
    Show that exec() can fail, for at least 2 different reasons
    exec would fail if:
        - The file to execute does not exist.
        - The file exists but is not executable or lacks permissions
*/
#include<stdio.h>
#include<unistd.h>
int main()
{
    printf("The file to execute does not exist");
    if(execl("/np", "np", NULL) == -1)
        perror("not found");

    printf("The file exists but is not executable or lacks permissions");
    if (execl("./no_permission.sh", "no_permission.sh", NULL) == -1) 
    {
        perror("no execute permission");
    }
    return 0;
}