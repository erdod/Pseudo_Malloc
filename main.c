#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include "pseudo_malloc.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 4        
#define MEMORY_SIZE 1024     
#define NUM_LEVELS 4          
#define MIN_BUCKET_SIZE 64   
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

uint8_t *buffer;            
char memory[MEMORY_SIZE];   

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <nome_risorsa>\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char* resource_name = argv[1];
    BuddyAllocator alloc;
    bitmap bitmap_struct;
    buffer = (uint8_t*)malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Errore allocazione buffer");
        return EXIT_FAILURE;
    }
    int req_size = get_bytes(NUM_LEVELS);
    if (req_size > BUFFER_SIZE) {
        printf("Errore: il buffer di memoria non è sufficiente. Dimensione richiesta: %d, BUFFER_SIZE: %d\n", req_size, BUFFER_SIZE);
        free(buffer);
        return EXIT_FAILURE;
    }
    printf("Dimensione richiesta: %d, BUFFER_SIZE: %d\n", req_size);
    buddy_allocator_init(&alloc, &bitmap_struct, memory, buffer, NUM_LEVELS, MIN_BUCKET_SIZE);
    printf("Inizializzazione COMPLETATA\n");
    void* b1 = pseudo_malloc(&alloc, 128, resource_name);
    void* b2 = pseudo_malloc(&alloc, 128, resource_name);
    void* b3 = pseudo_malloc(&alloc, 256, resource_name);
    printf("Bitmap dopo le allocazioni con richiesta gestita tramite BuddyAllocator:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    pseudo_free(&alloc, b1, 128, resource_name);
    pseudo_free(&alloc, b2, 128, resource_name);
    pseudo_free(&alloc, b3, 256, resource_name);
    printf("Bitmap dopo le deallocazioni con richiesta gestita tramite BuddyAllocator:\n");
    for (int i = 0; i < alloc.bit_map.num_bit; i++) {
        printf("%d", get_status(&alloc.bit_map, i));
    }
    printf("\n");
    void* large_block1 = pseudo_malloc(&alloc, PAGE_SIZE / 2, resource_name);  
    void* large_block2 = pseudo_malloc(&alloc, PAGE_SIZE, resource_name);      
    pseudo_free(&alloc, large_block1, PAGE_SIZE / 2, resource_name);
    pseudo_free(&alloc, large_block2, PAGE_SIZE, resource_name);
    printf("Test completato con successo.\n");
    free(buffer);
    return EXIT_SUCCESS;
}
