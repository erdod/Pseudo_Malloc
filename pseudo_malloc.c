#include "pseudo_malloc.h"
#include "buddy_allocator.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define PAGE_SIZE sysconf(_SC_PAGESIZE)

void* pseudo_malloc(BuddyAllocator* alloc, size_t size, const char* resource_name) {
    size_t threshold = PAGE_SIZE / 4;
    printf("pseudo_malloc chiamato con richiesta di %zu bytes.\n", size);
    printf("Threshold per allocazioni grandi è: %zu bytes.\n", threshold);
    if (size >= threshold) {
        printf("Dimensione richiesta è >= al threshold. Procedo con shm_open e mmap.\n");
        int fd = shm_open(resource_name, O_CREAT | O_RDWR, 0666);
        if (fd < 0) {
            perror("Errore shm_open");
            return NULL;
        }
        if (ftruncate(fd, size) == -1) {
            perror("Errore ftruncate");
            shm_unlink(resource_name); 
            return NULL;
        }

        void* shared_alloc = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (shared_alloc == MAP_FAILED) {
            perror("Errore mmap");
            shm_unlink(resource_name); 
            return NULL;
        }
        printf("Allocato grande blocco tramite mmap a: %p di dimensione: %zu bytes\n", shared_alloc, size);
        return shared_alloc;
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

void pseudo_free(BuddyAllocator* alloc, void* ptr, size_t size, const char* resource_name) {
    size_t threshold = PAGE_SIZE / 4;
    if (size >= threshold) {
        if (munmap(ptr, size) == 0) {
            printf("Blocco grande deallocato con successo tramite munmap: %p\n", ptr);
        } else {
            perror("munmap");
        }
    } else {
        printf("Deallocazione di un blocco tramite BuddyAllocator: %p\n", ptr);
        BuddyListItem* buddy = *(BuddyListItem**)((char*)ptr - 8);
        int buddy_index = buddy->idx;
        printf("L'indice per il buddy calcolato è: %d\n", buddy_index);
        BuddyAllocator_free(alloc, buddy_index);
    }
}
