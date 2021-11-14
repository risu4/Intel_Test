#include <stdio.h>
#include <stdint.h>
#include <iostream>

using namespace std;
int main(void)
{
   int16_t i = 2;
   int8_t *p = (int8_t *) &i;
   *p = 100;
   cout<<to_string(*p)<<endl;
   if (p[0] == 1) printf("Little Endian\n");
   else           printf("Big Endian\n");

   return 0;
}