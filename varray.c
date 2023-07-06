#include "varray.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* _VARRAY_INIT(size_t size, size_t stride)
{
        size_t header_size = VARRAY_HEADER_LENGTH * sizeof(uint64_t);
        size_t array_size = size * stride;
        uint64_t* varray = malloc(header_size + array_size);
        varray[VARRAY_SIZE] = size;
        varray[VARRAY_LENGTH] = 0;
        varray[VARRAY_STRIDE] = stride;
        return (void*)(varray + VARRAY_HEADER_LENGTH);
}
void _VARRAY_DESTROY(void* varray)
{
        uint64_t* header = (uint64_t*)varray - VARRAY_HEADER_LENGTH;
        //uint64_t header_size = VARRAY_HEADER_LENGTH * sizeof(uint64);
        //uint64_t total_size = header_size + (header[VARRAY_SIZE] * header[VARRAY_STRIDE]);
        free(header);
}

uint64_t _VARRAY_HEADER_GET(void* varray, VARRAY_FIELD header_field)
{
        uint64_t* header = (uint64_t*)varray - VARRAY_HEADER_LENGTH;
        return header[header_field];
}

void _VARRAY_HEADER_SET(void* varray, VARRAY_FIELD header_field, uint64_t header_value)
{
        uint64_t* header = (uint64_t*)varray - VARRAY_HEADER_LENGTH;
        header[header_field] = header_value;
}

void* _VARRAY_RESIZE(void* varray, int16_t resize_factor)
{
        uint64_t* header = (uint64_t*)varray - VARRAY_HEADER_LENGTH;

        uint64_t new_size = 0;
        uint64_t* new_varray;

        if(resize_factor > 0) //increase varray size
        {
                new_size = header[VARRAY_SIZE] * resize_factor; 
                new_varray = _VARRAY_INIT(new_size,header[VARRAY_STRIDE]);
                memcpy(new_varray,varray,header[VARRAY_SIZE]*header[VARRAY_STRIDE]);
        }
        else //decrease varray size
        {
                new_size = header[VARRAY_SIZE] / abs(resize_factor); 
                new_varray = _VARRAY_INIT(new_size,header[VARRAY_STRIDE]);
                memcpy(new_varray,varray,new_size*header[VARRAY_STRIDE]);
        }

        //set length of new varray from previous
        _VARRAY_HEADER_SET(new_varray, VARRAY_LENGTH, VARRAY_LENGTH_GET(varray));

        _VARRAY_DESTROY(varray);

        return (void*)(new_varray);
}

void* _VARRAY_RESERVE(void* varray, uint64_t new_size)
{
        uint64_t current_size = VARRAY_SIZE_GET(varray);
        if(current_size > new_size) return varray;

        uint64_t resize_factor = 1;
        while(current_size < new_size)
        {
                current_size = current_size * VARRAY_RESIZE_FACTOR;         
                resize_factor = resize_factor * VARRAY_RESIZE_FACTOR; 
        }

        varray = _VARRAY_RESIZE(varray,resize_factor);

        return varray;

        
}

void* _VARRAY_PUSH(void* varray, const void* value)
{
        uint64_t length = VARRAY_LENGTH_GET(varray);
        uint64_t stride = VARRAY_STRIDE_GET(varray);
        uint64_t size = VARRAY_SIZE_GET(varray);

        //resize if length will be larger then size
        if(length + 1 > size)
                varray = _VARRAY_RESIZE(varray, VARRAY_RESIZE_FACTOR);

        uint64_t address = (uint64_t)varray + length * stride;
        memcpy((void*)address,value,stride);
        
        _VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length+1);
        return varray;
}

void* _VARRAY_POP(void* varray, void* dest)
{
        uint64_t size = VARRAY_SIZE_GET(varray);  
        uint64_t length = VARRAY_LENGTH_GET(varray);  
        uint64_t stride = VARRAY_STRIDE_GET(varray);  

        uint64_t address = (uint64_t)varray + (length-1) * stride;
        if(dest != NULL)
                memcpy(dest, (void*)address,stride);

        if(length-1 < size/VARRAY_SHRINK_THRESHOLD)
                varray = _VARRAY_RESIZE(varray,VARRAY_RESIZE_FACTOR*-1);

        _VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length-1);

        return varray;
}

void* _VARRAY_INSERT(void* varray, uint64_t index, const void* value)
{
        uint64_t length = VARRAY_LENGTH_GET(varray);
        uint64_t stride = VARRAY_STRIDE_GET(varray);
        uint64_t size = VARRAY_SIZE_GET(varray);

        if(index>size) return varray;

        if(length+1 > size)
                varray = _VARRAY_RESIZE(varray,VARRAY_RESIZE_FACTOR);

        uint64_t start_address = (uint64_t)varray + index * stride;
        uint64_t target_address = (uint64_t)varray + (index + 1) * stride;
        uint64_t amount_of_data = (length - index) * stride;

        memcpy((void*)target_address,(void*)start_address,amount_of_data); //shift data over

        memcpy((void*)start_address,value,stride); //insert data at the index
        
        _VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length+1);
        return varray;
}

void* _VARRAY_ERASE(void* varray, uint64_t index)
{
        uint64_t length = VARRAY_LENGTH_GET(varray);
        uint64_t stride = VARRAY_STRIDE_GET(varray);
        uint64_t size = VARRAY_SIZE_GET(varray);

        if(index>size) return varray;

        uint64_t start_address = (uint64_t)varray + (index + 1) * stride;
        uint64_t target_address = (uint64_t)varray + index * stride;
        uint64_t amount_of_data = (length - (index+1)) * stride;

        memcpy((void*)target_address,(void*)start_address,amount_of_data); //shift data over
        
        if(length-1 < size/VARRAY_SHRINK_THRESHOLD)
                varray = _VARRAY_RESIZE(varray,VARRAY_RESIZE_FACTOR*-1);
        
        _VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length-1);
        return varray;
}

void _VARRAY_SWAP(void* varray, uint64_t index1, uint64_t index2)
{
        uint64_t length = VARRAY_LENGTH_GET(varray);
        uint64_t stride = VARRAY_STRIDE_GET(varray);
        
        if(index1 > length || index2 > length) return;
        
        void* buffer = malloc(stride);

        uint64_t address1 = (uint64_t)varray + index1 * stride;
        uint64_t address2 = (uint64_t)varray + index2 * stride;

        memcpy(buffer,(void*)address1,stride);
        memcpy((void*)address1,(void*)address2,stride);
        memcpy((void*)address2,buffer,stride);

		free(buffer);
}

void* _VARRAY_FREE(void** varray, uint64_t index)
{
    free(varray[index]);
    VARRAY_ERASE(varray,index);
    return varray;
}
