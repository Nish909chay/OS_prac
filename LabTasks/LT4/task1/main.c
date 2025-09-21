// To show that C programs can be modularized, compiled separately, and then combined.

#include<stdio.h>
int add(int a, int b);
int sub(int a, int b);
int main()
{
    printf("%d", add(10, 20));
    printf("\n %d", add(30, 20));

    return 0;
}

/*
make obj files
gcc -c main.c  
gcc -c a.c      
gcc -c b.c 
*/