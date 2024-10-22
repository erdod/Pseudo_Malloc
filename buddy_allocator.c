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

    for (int i = 0; i < buddy_allocator->num_levels; i++) {
        List_init(&buddy_allocator->free[i]);
    }

    BuddyListItem* initial_item = BuddyAllocator_createListItem(buddy_allocator, 0, NULL); 
    List_push_item(&buddy_allocator->free[0], (ListItem*)initial_item);  
}


void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
    int level = (int)ceil(log2((size + 8) / alloc->min_bucket_size));
    
    if (level > alloc->num_levels) {
        level = alloc->num_levels; 
    }

    printf("Requested size: %d, Calculated level: %d\n", size, level);
    
    BuddyListItem* buddy = BuddyAllocator_getBuddy(alloc, level);
    if (!buddy) {
        printf("No buddy available at level %d\n", level);
        return NULL; 
    }

    int buddy_index = buddy->idx;
    set_bit(&alloc->bit_map, buddy_index, STATUS_ON);

    BuddyListItem** target = (BuddyListItem**)(buddy->start);
    *target = buddy;
    printf("Buddy allocated at: %p\n", buddy->start);
    
    return buddy->start + 8;  
}


void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
    char* p = (char*)mem;
    p -= 8;  
    BuddyListItem** buddy_ptr = (BuddyListItem**)p;
    BuddyListItem* buddy = *buddy_ptr;
    BuddyAllocator_releaseBuddy(alloc, buddy);
    int buddy_index = buddy->idx;
    set_bit(&alloc->bit_map, buddy_index, STATUS_OFF);
}


void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, BuddyListItem* item) {
    BuddyListItem* parent_ptr = item->parent_ptr;
    BuddyListItem* buddy_ptr = item->buddy_ptr;
    List_push_item(&alloc->free[item->level], (ListItem*)item);
    if (!parent_ptr) {
        return;
    }
    if (buddy_ptr->list.prev == NULL && buddy_ptr->list.next == NULL) {
        return;
    }
    printf("Merging blocks at level %d\n", item->level);
    int item_index = item->idx;
    set_bit(&alloc->bit_map, item_index, STATUS_OFF);
    int buddy_index = buddy_ptr->idx;
    set_bit(&alloc->bit_map, buddy_index, STATUS_OFF);
    BuddyAllocator_destroyListItem(alloc, item);       
    BuddyAllocator_destroyListItem(alloc, buddy_ptr);  
    BuddyAllocator_releaseBuddy(alloc, parent_ptr);
}


BuddyListItem* BuddyAllocator_createListItem(BuddyAllocator* alloc, int idx, BuddyListItem* parent) {
    BuddyListItem* item = (BuddyListItem*)malloc(sizeof(BuddyListItem));
    if (!item) {
        return NULL; 
    }
    item->idx = idx;
    item->parent_ptr = parent;
    item->buddy_ptr = NULL; 
    item->size = alloc->min_bucket_size << (alloc->num_levels - idx);
    item->start = alloc->mem + (idx * item->size);

    return item;
}

void BuddyAllocator_destroyListItem(BuddyAllocator* alloc, BuddyListItem* item) {
    free(item); 
}


BuddyListItem* BuddyAllocator_getBuddy(BuddyAllocator* alloc, int level) {
    if (level < 0) {
        printf("Requested level %d is negative, returning NULL.\n", level);
        return NULL;  
    }

    // Controlla se ci sono elementi disponibili nel livello richiesto
    if (!List_isEmpty(&alloc->free[level])) {
        printf("Found buddy at level %d, retrieving item.\n", level);
        BuddyListItem* item = (BuddyListItem*)List_remove_item(&alloc->free[level]);
        printf("Retrieved buddy at address: %p\n", (void*)item->start);
        return item;
    }

    // Se non ci sono buddy nel livello attuale, cerca nel livello superiore
    printf("No buddy available at level %d, checking level %d.\n", level, level - 1);
    BuddyListItem* parent_ptr = BuddyAllocator_getBuddy(alloc, level - 1);
    if (!parent_ptr) {
        printf("No buddy found at level %d.\n", level - 1);
        return NULL; 
    }

    // Calcola gli indici dei buddy
    int left_idx = parent_ptr->idx * 2;
    int right_idx = left_idx + 1;

    printf("Creating left and right buddies for parent at index %d.\n", parent_ptr->idx);
    BuddyListItem* left_ptr = BuddyAllocator_createListItem(alloc, left_idx, parent_ptr);
    BuddyListItem* right_ptr = BuddyAllocator_createListItem(alloc, right_idx, parent_ptr);

    left_ptr->buddy_ptr = right_ptr;
    right_ptr->buddy_ptr = left_ptr;

    // Aggiunge il buddy destro alla lista di free
    List_push_item(&alloc->free[level], (ListItem*)right_ptr);
    printf("Pushed buddy at address: %p to level %d.\n", (void*)right_ptr->start, level);

    // Restituisce il pointer al buddy sinistro
    printf("Returning left buddy at address: %p.\n", (void*)left_ptr->start);
    return left_ptr;
}










