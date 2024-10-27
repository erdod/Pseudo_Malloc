#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>

#define BUFFER_SIZE 4        
#define MEMORY_SIZE 1024     
#define NUM_LEVELS 4          
#define MIN_BUCKET_SIZE 64   
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

// Buffer per il Buddy Allocator
uint8_t *buffer;            
char memory[MEMORY_SIZE];   

void* pseudo_malloc(BuddyAllocator* alloc, size_t size) {
    size_t threshold = PAGE_SIZE / 4;
    printf("pseudo_malloc chiamato con richiesta di %zu bytes.\n", size);
    printf("Threshold per allocazioni grandi è: %zu bytes.\n", threshold);
    if (size >= threshold) {
        printf("Dimensione richiesta è >= al threshold. Procedo con mmap.\n");
        void* large_alloc = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        if (large_alloc == MAP_FAILED) {
            perror("Errore mmap");
            return NULL;
        }
        printf("Allocato grande blocco tramite mmap a: %p di dimensione: %zu bytes\n", large_alloc, size);
        return large_alloc;
    } else {
        printf("Dimensione richiesta è < al threshold. Procedo con BuddyAllocator.\n");
        printf("Chiamata a BuddyAllocator_malloc con dimensione: %zu bytes\n", size);
        void* small_alloc = BuddyAllocator_malloc(alloc, size);
        if (!small_alloc) {
            printf("Allocazione fallita per %zu bytes con BuddyAllocator.\n", size);
        } else {
            printf("Allocazione completata tramite BuddyAllocator. Indirizzo del blocco allocato: %p\n", small_alloc);
        }
        return small_alloc;
    }
}


void pseudo_free(BuddyAllocator* alloc, void* ptr, size_t size) {
    size_t threshold = PAGE_SIZE / 4;
    if (size >= threshold) {
        if (munmap(ptr, size) == 0) {
            printf("Blocco grande deallocato con successo tramite munmap: %p\n", ptr);
        } else {
            perror("munmap");
        }
    } else {
        printf("Deallocazione di un blocco tramite BuddyAllocator: %p\n", ptr);
        int buddy_index = ((char*)ptr - (char*)alloc->mem - 8) / alloc->min_bucket_size;
        BuddyAllocator_free(alloc, buddy_index);
    }
}

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
    void* b1 = pseudo_malloc(&alloc, 128);
    void* b2 = pseudo_malloc(&alloc, 128);
    void* b3 = pseudo_malloc(&alloc, 256);
    printf("Bitmap dopo le allocazioni con richiesta gestita tramite BuddyAllocator:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    pseudo_free(&alloc, b1, 128);
    pseudo_free(&alloc, b2, 128);
    pseudo_free(&alloc, b3, 256);
    printf("Bitmap dopo le deallocazioni con richiesta gestita tramite BuddyAllocator:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    void* large_block1 = pseudo_malloc(&alloc, PAGE_SIZE / 2);  
    void* large_block2 = pseudo_malloc(&alloc, PAGE_SIZE);      
    pseudo_free(&alloc, large_block1, PAGE_SIZE / 2);
    pseudo_free(&alloc, large_block2, PAGE_SIZE);
    printf("Test completato con successo.\n");
    return 0;
}
