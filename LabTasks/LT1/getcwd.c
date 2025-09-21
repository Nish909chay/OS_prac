#include<stdio.h>
#include<unistd.h>  // provides access to POSIX OS 
#include<limits.h>  // to set maximum/ minimum values for data types and system calls
int main() 
{
    char cwd[PATH_MAX];

    if(getcwd(cwd, sizeof(cwd)) != NULL )
    {
        printf("CWD Path %s \n", cwd);
    }
    else
    {
        perror("error");
    }

    // chdir - system call that changes cwd to path specified(absolute ot relative)
    if(chdir("..") == 0)   // ".." means go to parent dir
    {
        if(getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("new dir: %s  \n", cwd);
        }
        else
        {
            perror("error");
        }
    }
    return 0;
}