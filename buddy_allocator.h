#include "bitmap.h"

typedef struct {
  bitmap bit_map;
  uint8_t* mem;
  int num_levels;
  int min_bucket_size;
} BuddyAllocator;

void BuddyAllocator_init(BuddyAllocator* buddyallocator, int total_memory_size, int min_bucket_size);

int BuddyAllocator_alloc(BuddyAllocator* buddyallocator, int size);

void BuddyAllocator_free(BuddyAllocator* buddyallocator, int index);

int BuddyAllocator_get_block_size(BuddyAllocator* buddyallocator, int level);

int BuddyAllocator_is_block_free(BuddyAllocator* buddyallocator, int index);


