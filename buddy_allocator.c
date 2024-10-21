#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "buddy_allocator.h"

int get_level(int index) {
    int level = 0;
    while ((1 << (level + 1)) - 1 <= index) {
        level++;
    }
    return level;
}

int get_parent(int index) {
    if (index == 0) {
        return -1; 
    }
    return (index - 1) / 2;
}

int get_buddy(int index) {
    if (index % 2 == 0) {
        return index - 1; 
    } else {
        return index + 1;  
    }
}

void buddy_allocator_init(BuddyAllocator *buddy_allocator, bitmap *bitmap, char *mem, int num_levels, int min) {
    buddy_allocator->mem = mem;        
    buddy_allocator->num_levels = num_levels;
    buddy_allocator->min_bucket_size = min;

    int num_blocks = (1 << buddy_allocator->num_levels) - 1;

    uint8_t* buffer = (uint8_t*)malloc(get_bytes(num_blocks));  
    bitmap_init(bitmap, buffer, num_blocks);  

    buddy_allocator->bit_map = *bitmap;   

    for (int i = 0; i < num_blocks; i++) {
        set_bit(&buddy_allocator->bit_map, i, STATUS_OFF);  
    }
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
    int level = (int)ceil(log2((size + 8) / alloc->min_bucket_size));
    
    if (level > alloc->num_levels) {
        level = alloc->num_levels; 
    }

    BuddyListItem* buddy = BuddyAllocator_getBuddy(alloc, level);
    if (!buddy) {
        return NULL; 
    }

    BuddyListItem** target = (BuddyListItem**)(buddy->start);
    *target = buddy;
    return buddy->start + 8;  
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
    printf("freeing %p\n", mem);
    // Recupera il buddy dal sistema
    char* p = (char*)mem;
    p -= 8;  // Torna indietro per trovare il puntatore del buddy
    BuddyListItem** buddy_ptr = (BuddyListItem**)p;
    BuddyListItem* buddy = *buddy_ptr;

    // Controllo di integrità
    assert(buddy->start == p);

    // Libera il buddy
    BuddyAllocator_releaseBuddy(alloc, buddy);
}







