#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <stdint.h>
#include "bitmap.h"
#include "list_item.h"

#define MAX_LEVELS 100

typedef struct BuddyListItem {
    ListItem list;  
    int idx;          
    int level;        
    char* start;          
    int size;              
    struct BuddyListItem* buddy_ptr;
    struct BuddyListItem* parent_ptr; 
} BuddyListItem;

typedef struct {
    bitmap bit_map;         
    uint8_t* mem;           
    int num_levels;         
    int min_bucket_size;    
    ListHead free[MAX_LEVELS];   
    ListHead occupied[MAX_LEVELS];
} BuddyAllocator;

void buddy_allocator_init(BuddyAllocator *buddy_allocator, bitmap *bitmap, char *mem, uint8_t *buffer, int num_levels, int min);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);
void BuddyAllocator_free(BuddyAllocator* alloc, int index);
BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level);
int get_level(int index);
int get_parent(int index);
int get_buddy(int index);
BuddyListItem* BuddyAllocator_createListItem(BuddyAllocator* alloc, int idx, BuddyListItem* parent);
void BuddyAllocator_destroyListItem(BuddyAllocator* alloc, BuddyListItem* item);

#endif // BUDDY_ALLOCATOR_H
