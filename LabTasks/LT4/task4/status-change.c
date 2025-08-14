/*Wrrite a C program and demonstrate that it goes through following states using ps and /proc/status :  
RUNNING, 
SLEEPING, 
RUNNABLE, 
ZOMBIE */
#include<stdio.h>
#include<unistd.h>
int main()
{
    printf("here's the pid mate: %d \n", getpid());
    
    printf("going to SLEEPING state \n");
    sleep(10);

    printf("now Runnable: \n");
    for (volatile long i = 0; i < 1000000000; i++);

    printf("Becoming a zombie... \n");
    _exit(0);
    return 0;
}