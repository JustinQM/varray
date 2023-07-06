#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum VARRAY_FIELD
{
    VARRAY_LENGTH,
    VARRAY_SIZE,
    VARRAY_STRIDE,
    VARRAY_HEADER_LENGTH //always last
}VARRAY_FIELD;

void* _VARRAY_INIT(size_t size, size_t stride);
void _VARRAY_DESTROY(void* varray);

void* _VARRAY_RESIZE(void* varray, int16_t resize_factor);
void* _VARRAY_RESERVE(void* varray, size_t new_size);

uint64_t _VARRAY_HEADER_GET(void* varray, VARRAY_FIELD header_field);
void _VARRAY_HEADER_SET(void* varray, VARRAY_FIELD header_field, uint64_t header_value);

void* _VARRAY_PUSH(void* varray, const void* value);
void* _VARRAY_POP(void* varray, void* dest);

void* _VARRAY_INSERT(void* varray, uint64_t index, const void* value);
void* _VARRAY_ERASE(void* varray, uint64_t index);

void _VARRAY_SWAP(void* varray, uint64_t index1, uint64_t index2);

void* _VARRAY_FREE(void** varray, uint64_t index);

#ifndef VARRAY_DEFAULT_SIZE
#define VARRAY_DEFAULT_SIZE 1
#endif

#ifndef VARRAY_RESIZE_FACTOR
#define VARRAY_RESIZE_FACTOR 2
#endif

#ifndef VARRAY_SHRINK_THRESHOLD
#define VARRAY_SHRINK_THRESHOLD 3
#endif

#define VARRAY(type) _VARRAY_INIT(VARRAY_DEFAULT_SIZE, sizeof(type))
#define VARRAY_DESTROY(varray) _VARRAY_DESTROY(varray)

#define VARRAY_RESERVE(varray,new_size) varray = _VARRAY_RESERVE(varray,new_size)

#define VARRAY_SIZE_GET(varray) _VARRAY_HEADER_GET(varray, VARRAY_SIZE)
#define VARRAY_LENGTH_GET(varray) _VARRAY_HEADER_GET(varray, VARRAY_LENGTH)
#define VARRAY_STRIDE_GET(varray) _VARRAY_HEADER_GET(varray, VARRAY_STRIDE)

#define VARRAY_PUSH(varray,value)\
    {\
        typeof(*varray) typedvalue = value;\
        varray = _VARRAY_PUSH(varray,&typedvalue);\
    }\

#define VARRAY_POP(varray,dest) varray = _VARRAY_POP(varray,dest)

#define VARRAY_FRONT(varray) varray[0]
#define VARRAY_BACK(varray) varray[VARRAY_LENGTH_GET(varray)-1]

#define VARRAY_INSERT(varray,index,value)\
    {\
        typeof(*varray) typedvalue = value;\
        varray = _VARRAY_INSERT(varray,index,&typedvalue);\
    }\

#define VARRAY_ERASE(varray,index) varray = _VARRAY_ERASE(varray,index)

#define VARRAY_SWAP(varray,index1,index2) _VARRAY_SWAP(varray,index1,index2)

#define VARRAY_FREE(varray,index)\
    {\
        varray = _VARRAY_FREE((void**)varray,index);\
    }\
