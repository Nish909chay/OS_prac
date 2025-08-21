// gcc -S file.c - 
#include<stdio.h>

void reverse(int n)
{
    int rev = 0, get;
    printf("Before Reverse = %d \n", n);
    while(n > 0)
    {
        get = n % 10;
        rev = rev*10 + get;
        n /= 10;
    }
    printf("After Reverse = %d", rev);
}

int main()
{
    reverse(123);
    return 0;
}