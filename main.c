#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 3        
#define MEMORY_SIZE 1024    
#define NUM_LEVELS 4          
#define MIN_BUCKET_SIZE 64   

uint8_t *buffer;            
char memory[MEMORY_SIZE];  

int main(int argc, char** argv) {
    BuddyAllocator alloc;  
    bitmap bitmap_struct; 
    int req_size = get_bytes(NUM_LEVELS);
    if (req_size > BUFFER_SIZE) {
        printf("Errore: il buffer di memoria non è sufficiente. Dimensione richiesta: %d, BUFFER_SIZE: %d\n", req_size, BUFFER_SIZE);
        return 1;
    }
    printf("Dimensione richiesta: %d, BUFFER_SIZE: %d\n", req_size, BUFFER_SIZE);
    buddy_allocator_init(&alloc, &bitmap_struct, memory, buffer, NUM_LEVELS, MIN_BUCKET_SIZE);
    printf("Inizializzazione COMPLETATA\n");
    printf("Allocazione di 512 bytes...\n");
    void* p1 = BuddyAllocator_malloc(&alloc, 512);
    if (p1) {
        printf("Blocco allocato a: %p\n", p1);
    } else {
        printf("Allocazione fallita per 512 bytes\n");
    }
    printf("Bitmap dopo allocazione primo blocco:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    printf("Allocazione di 128 bytes...\n");
    void* p2 = BuddyAllocator_malloc(&alloc, 128);
    if (p2) {
        printf("Blocco allocato a: %p\n", p2);
    } else {
        printf("Allocazione fallita per 128 bytes\n");
    }
    printf("Bitmap dopo allocazione secondo blocco:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    printf("Allocazione di 256 bytes...\n");
    void* p3 = BuddyAllocator_malloc(&alloc, 256);
    if (p3) {
        printf("Blocco allocato a: %p\n", p3);
    } else {
        printf("Allocazione fallita per 256 bytes\n");
    }
    printf("Bitmap dopo allocazione terzo blocco:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    printf("Deallocazione del blocco p1...\n");
    BuddyAllocator_free(&alloc, 1);
    printf("Deallocazione del blocco p2...\n");
    BuddyAllocator_free(&alloc, 11);
    printf("Deallocazione del blocco p3...\n");
    BuddyAllocator_free(&alloc, 6);
    printf("Bitmap finale:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");

    return 0;
}
