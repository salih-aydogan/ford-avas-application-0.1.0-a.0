#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "test.h"

void function1(int param1,
                uint32_t param2, 
               char *p_array);

void function2(int param1,
                uint32_t param2, 
               char *p_array, 
               size_t size)
{
    if ((param1 > 5) && (param2 < 10))
    {

        size = param1;
    }

    size = param1 * param2
          ? 4582
           : size * param2;

    size =      
      param1;

    size = param2 +
          param1;


    if ((0 > 5) && 
       (1 < 10))
    {
        ;
    }
}
