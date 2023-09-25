#include "utilities.h"
#include <string.h>
#include <math.h>


void ByteToStr(uint8_t v,char str[4]){
  uint8_t i, rem, len = 0, n;
 
    n = v;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = v % 10;
        v= v / 10;
        str[len - (i + 1)] = rem + '0';
    }
	    str[len] = '\0';   
}