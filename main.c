#include <stdio.h>
#include <stdint.h>
#include "varray.h"

int main(void)
{
    uint64_t test_amount = 1000000;
    // uint64_t* test = VARRAY(uint64_t);
	uint64_t* test = VARRAY(uint64_t);

    for(int i = 0; i < test_amount; i++)
    {
        VARRAY_PUSH(test, i);
        printf("%lu\n",test[i]);
    }

    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);
    VARRAY_SWAP(test, 5, 10);

	VARRAY_DESTROY(test);
    return 0;
}

