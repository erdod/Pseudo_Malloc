#include "bitmap.h"

typedef struct {
  bitmap bit_map;
  uint8_t* mem;
  int num_levels;
  int min_bucket_size;
} BuddyAllocator;

void buddy_allocator_init(BuddyAllocator *buddy_allocator, bitmap *bitmap, char *memory, int lvl, int min); 

int BuddyAllocator_alloc(BuddyAllocator* buddyallocator, int size);

void BuddyAllocator_free(BuddyAllocator* buddyallocator, int index);

int BuddyAllocator_get_block_size(BuddyAllocator* buddyallocator, int level);

int BuddyAllocator_is_block_free(BuddyAllocator* buddyallocator, int index);

int get_level(int index);

int get_parent(int index);

int get_buddy(int index);


