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
        _VARRAY_HEADER_SET(new_varray, VARRAY_LENGTH, varray_length(varray));

        _VARRAY_DESTROY(varray);

        return (void*)(new_varray);
}

void* _VARRAY_RESERVE(void* varray, uint64_t new_size)
{
        uint64_t current_size = varray_size(varray);
        if(current_size > new_size) return varray;

        while(current_size < new_size)
        {
			varray = _VARRAY_RESIZE(varray,VARRAY_RESIZE_FACTOR);
			current_size = varray_size(varray);
        }

        return varray;

        
}

void* _VARRAY_PUSH(void* varray, const void* value)
{
        uint64_t length = varray_length(varray);
        uint64_t stride = varray_stride(varray);
        uint64_t size = varray_size(varray);

        //resize if length will be larger then size
        if(length + 1 > size)
                varray = _VARRAY_RESIZE(varray, VARRAY_RESIZE_FACTOR);

        uint64_t address = (uint64_t)varray + length * stride;
        memcpy((void*)address,value,stride);
        
        _VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length+1);
        return varray;
}

void* _VARRAY_PUSH_MANY(void* varray, ...)
{
		va_list ap;
		const void* value = NULL;
		va_start(ap,varray);
		value = va_arg(ap,const void*);
		while(value != NULL)
		{
			uint64_t length = varray_length(varray);
			uint64_t stride = varray_stride(varray);
			uint64_t size = varray_size(varray);

			//resize if length will be larger then size
			if(length + 1 > size)
			{
				varray = _VARRAY_RESIZE(varray, VARRAY_RESIZE_FACTOR);
			}

			uint64_t address = (uint64_t)varray + length * stride;
			memcpy((void*)address,&value,stride);

			_VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length+1);

			value = va_arg(ap,void*);
		}
		va_end(ap);
        return varray;
}
void* _VARRAY_POP(void* varray, void* dest)
{
        uint64_t size = varray_size(varray);  
        uint64_t length = varray_length(varray);  
        uint64_t stride = varray_stride(varray);  

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
        uint64_t length = varray_length(varray);
        uint64_t stride = varray_stride(varray);
        uint64_t size = varray_size(varray);

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
        uint64_t length = varray_length(varray);
        uint64_t stride = varray_stride(varray);
        uint64_t size = varray_size(varray);

        if(index>size) return varray;

        uint64_t start_address = (uint64_t)varray + (index + 1) * stride;
        uint64_t target_address = (uint64_t)varray + index * stride;
		uint64_t amount_of_data = 0;
		if(length - index == 0) amount_of_data = stride;
		else amount_of_data = (length - (index+1)) * stride;

        memcpy((void*)target_address,(void*)start_address,amount_of_data); //shift data over
        
        if(length-1 < size/VARRAY_SHRINK_THRESHOLD)
                varray = _VARRAY_RESIZE(varray,VARRAY_RESIZE_FACTOR*-1);
        
        _VARRAY_HEADER_SET(varray, VARRAY_LENGTH, length-1);
        return varray;
}

void _VARRAY_SWAP(void* varray, uint64_t index1, uint64_t index2)
{
        uint64_t length = varray_length(varray);
        uint64_t stride = varray_stride(varray);
        
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
    varray_erase(varray,index);
    return varray;
}
