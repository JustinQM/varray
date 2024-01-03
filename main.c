#include <stdio.h>
#include <stdint.h>
#include "varray.h"

int main(void)
{
    uint64_t test_amount = 100;
	Varray(uint64_t) test = NULL;

	varray_reserve(test,test_amount);
	varray_push_many(test,50,100,200,6969,5999);

	for(int i=0; i<varray_length(test); i++)
	{
		printf("%lu\n",test[i]);
	}

    for(int i = 0; i < test_amount; i++)
    {
        varray_push(test, i);
		printf("VARRAY LENGTH: %lu, VARRAY SIZE: %lu\n",varray_length(test),varray_size(test));
        // printf("%lu\n",test[i]);
    }
    for(int i = 0; i < test_amount; i++)
    {
        varray_erase(test, varray_length(test));
		printf("VARRAY LENGTH: %lu, VARRAY SIZE: %lu\n",varray_length(test),varray_size(test));
	}


	varray_destroy(test);
    return 0;
}

