#include <stdio.h>
#include <string.h>
#include "bitmap.h"

int main(){
    int num_bit = 16; 
    char buffer[2];   
    
    memset(buffer, 0, sizeof(buffer));
    
    bitmap my_bitmap;
    bitmap_init(&my_bitmap, buffer, num_bit);

    set_bit(&my_bitmap, 1, STATUS_ON);
    set_bit(&my_bitmap, 2, STATUS_OFF);
    set_bit(&my_bitmap, 8, STATUS_ON);
    set_bit(&my_bitmap, 16, STATUS_ON);

    printf("Bit 1: %d \n", get_status(&my_bitmap, 1));
    printf("Bit 2: %d \n", get_status(&my_bitmap, 2));
    printf("Bit 8: %d \n", get_status(&my_bitmap, 8));
    printf("Bit 16: %d \n", get_status(&my_bitmap, 16));
    

    return 0;
}
