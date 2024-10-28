#ifndef PSEUDO_MALLOC_H
#define PSEUDO_MALLOC_H

#include "buddy_allocator.h"
#include <stddef.h>

void* pseudo_malloc(BuddyAllocator* alloc, size_t size, const char* resource_name);
void pseudo_free(BuddyAllocator* alloc, void* ptr, size_t size, const char* resource_name);

#endif 
