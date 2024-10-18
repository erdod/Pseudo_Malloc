#include <stdio.h>
#include <stdlib.h>

#include "bitmap.h"

int get_bytes(int num_bit){
    return num_bit/8 + (((num_bit%8)!=0) ? 1 : 0);
}

void bitmap_init(bitmap* bitmap, uint8_t* buffer,int num_bit){
    bitmap->buffer=buffer;
    bitmap->num_bit= num_bit;
    bitmap->buffer_size= get_bytes(num_bit);
}

void set_bit(bitmap* bitmap, int bit, int status){
    int mask;
    int num_byte=get_bytes(bit);
    if((bit%8)==0){
        int shift= 7-(bit%8);
        mask= 1<<shift;
    }
    else{
        int shift= (bit%8)-1;
        mask = 1<<shift;
    }
    if(bit>bitmap->num_bit){
        fprintf(stdout,"il bit che si vuole settare supera le dimensioni della bitmap");
        exit(1);
    }
    if(status!= STATUS_ON && status!= STATUS_OFF){
        fprintf(stdout,"Il settaggio del bit deve essere 0 o 1");
        exit(1);
    }
    if(status==STATUS_ON)
        bitmap->buffer[num_byte] |= mask;
    else
        bitmap->buffer[num_byte] &= ~mask;

}

int get_status(bitmap* bitmap,int bit){
    int num_byte=get_bytes(bit);
    int mask;
        if((bit%8)==0){
        int shift= 7-(bit%8);
        mask= 1<<shift;
    }
    else{
        int shift= (bit%8)-1;
        mask = 1<<shift;
    }
    int res = bitmap->buffer[num_byte] & mask;
    if(res!=0)
        return 1;
    return 0;
}