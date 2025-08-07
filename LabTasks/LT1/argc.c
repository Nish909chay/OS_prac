#include<stdio.h>
// each element in array is a pointer that points to first char of the string
int main(int argc, char *argv[])    // pointer to a pointer to a char - array of char* pointers  
{
    printf("total arguments - %d \n", argc);
    printf("the arguments are : \n");
    for(int i = 0; i <  argc; i++)
    {
        printf("%d - %s \n", (i+1), argv[i]);
    }
    return 0;
}