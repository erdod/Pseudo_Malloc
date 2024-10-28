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
    return index ^ 1;  
}

void buddy_allocator_init(BuddyAllocator *buddy_allocator, bitmap *bitmap, char *mem, uint8_t *buffer, int num_levels, int min) {

    buddy_allocator->mem = mem;        
    buddy_allocator->num_levels = num_levels;
    buddy_allocator->min_bucket_size = min;
    int num_blocks = (1 << buddy_allocator->num_levels) - 1;
    printf("Numero totale di blocchi: %d\n", num_blocks);
    if (buffer == NULL) {
        buffer = (uint8_t*)malloc(get_bytes(num_blocks));
        if (buffer == NULL) {
            fprintf(stderr, "Errore: allocazione della memoria per la bitmap fallita!\n");
            exit(EXIT_FAILURE);
        }
        printf("Buffer allocato per la bitmap di %d byte\n", get_bytes(num_blocks));
    }
    bitmap_init(bitmap, buffer, num_blocks);
    printf("Bitmap inizializzata.\n");
    buddy_allocator->bit_map = *bitmap;   
    for (int i = 0; i < num_blocks; i++) {
        set_bit(&buddy_allocator->bit_map, i, STATUS_OFF);
        printf("Bit %d impostato su STATUS_OFF\n", i);
    }
    for (int i = 0; i < buddy_allocator->num_levels; i++) {
        List_init(&buddy_allocator->free[i]);
        printf("Lista per livello %d inizializzata.\n", i);
    }
    BuddyListItem* initial_item = BuddyAllocator_createListItem(buddy_allocator, 0, NULL);
    if (initial_item == NULL) {
        fprintf(stderr, "Errore: creazione dell'elemento iniziale della lista fallita!\n");
        exit(EXIT_FAILURE);
    }
    printf("Elemento iniziale della lista creato.\n");
    List_push_item(&buddy_allocator->free[0], (ListItem*)initial_item);
    printf("Elemento iniziale aggiunto alla lista di blocchi liberi per livello 0.\n");
}

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
    int level = alloc->num_levels;
    int bucket_size = alloc->min_bucket_size;
    while (bucket_size < size) {
        bucket_size <<= 1;
        level--;
    }
    if (level >= alloc->num_levels) {
        printf("Requested size exceeds maximum level\n");
        return NULL;
    }
    printf("Requested size: %d, Calculated level: %d\n", size, level);
    BuddyListItem* buddy = BuddyAllocator_getBuddy(alloc, level);
    if (!buddy) {
        printf("No buddy available at level %d\n", level);
        return NULL;
    }
    set_bit(&alloc->bit_map, buddy->idx, STATUS_ON);
    printf("Bit for buddy index %d set to STATUS_ON.\n", buddy->idx);
    BuddyListItem** target = (BuddyListItem**)(buddy->start);
    *target = buddy;  
    return buddy->start + 8;
}



void BuddyAllocator_free(BuddyAllocator* alloc, int index) {
    if (index < 0 || index >= alloc->bit_map.num_bit) {
        printf("Errore: Indice non valido per la deallocazione: %d\n", index);
        return;
    }
    set_bit(&alloc->bit_map, index, STATUS_OFF);
    printf("Bit per l'indice %d impostato su STATUS_OFF.\n", index);
    int current_index = index;
    int current_level = get_level(index);
    printf("livello del buddy da deallocare dato da get_levels() è: %d\n",current_level);  
    while (current_level < alloc->num_levels -1) {
        int buddy_index = get_buddy(current_index); 
        if (get_status(&alloc->bit_map, buddy_index) == STATUS_ON) {
            break; 
        }
        ListItem* removed_item = List_remove_item(&alloc->free[current_level]);
        if (removed_item) {
            printf("Rimosso buddy libero a indice %d dalla lista di livello %d.\n", buddy_index, current_level);
            free(removed_item);  
        } else {
            printf("Nessun buddy libero trovato a livello %d per l'indice %d.\n", current_level, buddy_index);
        }
        current_index = get_parent(current_index);
        current_level++;
    }
    BuddyListItem* merged_buddy = BuddyAllocator_createListItem(alloc, current_index, NULL);
    List_push_item(&alloc->free[current_level], (ListItem*)merged_buddy);
    printf("Blocco unito aggiunto alla lista di blocchi liberi a livello %d con indice %d.\n", current_level, current_index);
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
        printf("Requested level %d is negative\n", level);
        return NULL;  
    }
    if (!List_isEmpty(&alloc->free[level])) {
        printf("Found buddy at level %d", level);
        BuddyListItem* item = (BuddyListItem*)List_remove_item(&alloc->free[level]);
        printf("idx di item è: %d",item->idx);
        printf("Retrieved buddy at address: %p\n", (void*)item->start);
        return item;
    }
    printf("No buddy available at level %d, checking level %d.\n", level, level - 1);
    BuddyListItem* parent_ptr = BuddyAllocator_getBuddy(alloc, level - 1);
    if (!parent_ptr) {
        printf("No buddy found at level %d.\n", level - 1);
        return NULL; 
    }
    int left_idx = parent_ptr->idx * 2 + 1; 
    int right_idx = parent_ptr->idx * 2 + 2;
    printf("Creating left and right buddies for parent at index %d.\n", parent_ptr->idx);
    BuddyListItem* left_ptr = BuddyAllocator_createListItem(alloc, left_idx, parent_ptr);
    BuddyListItem* right_ptr = BuddyAllocator_createListItem(alloc, right_idx, parent_ptr);
    left_ptr->buddy_ptr = right_ptr;
    right_ptr->buddy_ptr = left_ptr;
    List_push_item(&alloc->free[level], (ListItem*)right_ptr);
    printf("Pushed buddy at address: %p to level %d.\n", (void*)right_ptr->start, level);
    printf("Returning left buddy at address: %p.\n", (void*)left_ptr->start);
    return left_ptr;
}
