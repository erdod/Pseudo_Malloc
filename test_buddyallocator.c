#include <stdio.h>
#include <stdlib.h>
#include "buddy_allocator.h"

#define MEMORY_SIZE 1024  // Dimensione della memoria totale per l'allocatore
#define MIN_BUCKET_SIZE 16  // Dimensione minima di un blocco
#define NUM_LEVELS 6  // Numero di livelli dell'albero dei buddy

int main() {
    uint8_t mem[MEMORY_SIZE];

    uint8_t buffer[(1 << NUM_LEVELS) - 1];

    bitmap bitmap;
    bitmap_init(&bitmap, buffer, (1 << NUM_LEVELS) - 1);

    BuddyAllocator buddy_allocator;
    buddy_allocator_init(&buddy_allocator, &bitmap, (char*)mem, NUM_LEVELS, MIN_BUCKET_SIZE);

    void* b1 = BuddyAllocator_malloc(&buddy_allocator, 64);
    void* b2 = BuddyAllocator_malloc(&buddy_allocator, 128);
    void* b3 = BuddyAllocator_malloc(&buddy_allocator, 32);

    printf("Blocco 1 allocato a: %p\n", b1);
    printf("Blocco 2 allocato a: %p\n", b2);
    printf("Blocco 3 allocato a: %p\n", b3);

    printf("Bitmap dopo le allocazioni:\n");
    for (int i = 0; i < (1 << NUM_LEVELS) - 1; i++) {
        printf("%d", get_status(&buddy_allocator.bit_map, i));
    }
    printf("\n");
 
    BuddyAllocator_free(&buddy_allocator, b1);

    printf("Bitmap dopo le liberazioni:\n");
    for (int i = 0; i < (1 << NUM_LEVELS) - 1; i++) {
        printf("%d", get_status(&buddy_allocator.bit_map, i));
    }
    printf("\n");

    void* block4 = BuddyAllocator_malloc(&buddy_allocator, 128);
    printf("Blocco 4 allocato a: %p\n", block4);

    printf("Bitmap finale:\n");
    for (int i = 0; i < (1 << NUM_LEVELS) - 1; i++) {
        printf("%d", get_status(&buddy_allocator.bit_map, i));
    }
    printf("\n");

    return 0;
}
