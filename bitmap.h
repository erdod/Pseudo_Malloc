#ifndef BITMAP_H  
#define BITMAP_H

#include <stdint.h>

#ifndef STATUS_H
#define STATUS_H

#define STATUS_ON  1
#define STATUS_OFF 0

#endif // STATUS_H

typedef struct bitmap
{
    uint8_t *buffer;
    int buffer_size;
    int num_bit;
}bitmap;

int get_bytes(int num_bit);

void bitmap_init(bitmap* bitmap,uint8_t  *buffer, int numbit);

void set_bit(bitmap* bitmap, int bit, int status);

int get_status(bitmap* bitmap, int bit);

#endif 


