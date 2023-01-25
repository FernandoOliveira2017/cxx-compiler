#include <stdlib.h>
#include <string.h>
#include "memory.h"

void *
cc_alloc(size_t size)
{
    void * pointer;
    
    pointer = malloc(size);
    if (pointer == NULL)
        error("failed to allocate");
	bzero(pointer, size);
	
	return pointer;
}

void *
cc_realloc(void * pointer, size_t size)
{
    pointer = realloc(pointer, size);
    if (pointer == NULL)
        error("failed to reallocate");
    return pointer;
}

void
cc_free(void * pointer)
{
    if (pointer != NULL)
        free(pointer);
}
