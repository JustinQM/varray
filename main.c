#include <stdio.h>
#include <stdint.h>
#include "varray.h"

int main(void)
{
    uint64_t test_amount = 100000;
    // uint64_t* test = VARRAY(uint64_t);
	VARRAY(uint64_t) test = NULL;

    for(int i = 0; i < test_amount; i++)
    {
        VARRAY_PUSH(test, i);
		printf("VARRAY LENGTH: %lu, VARRAY SIZE: %lu\n",VARRAY_LENGTH_GET(test),VARRAY_SIZE_GET(test));
        // printf("%lu\n",test[i]);
    }
    for(int i = 0; i < test_amount; i++)
    {
        VARRAY_ERASE(test, VARRAY_LENGTH_GET(test));
		printf("VARRAY LENGTH: %lu, VARRAY SIZE: %lu\n",VARRAY_LENGTH_GET(test),VARRAY_SIZE_GET(test));
	}


	VARRAY_DESTROY(test);
    return 0;
}

