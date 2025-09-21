/*
#include<stdio.h>

int main()
{
  FILE *fp;
  fp = fopen("mis_ids2.txt", "w");
  
  for(int i=1; i<100; i++)
  {
    fprintf(fp, "6424030%02d \n", i);
  }
  
  fclose(fp);
  printf("Done check the file");
  
  return 0;
}
*/

#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int main()
{
  FILE *fp = fopen("mis_ids2", "w");
  int i, r;
  srand(time(NULL));
  
  for(i = 0; i<21;i++)
  {
    r = rand() % 99 + 1;
    fprintf(fp, "6424030%02d \n", r);
  }
  
  fclose(fp);
  printf("Done");
}
