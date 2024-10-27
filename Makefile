CC = gcc
CFLAGS = -Wall -Wextra -lm
OBJ = main.o buddy_allocator.o pseudo_malloc.o bitmap.o list_item.o
TARGET = buddy_test
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $<
clean:
	rm -f $(OBJ) $(TARGET)
.PHONY: all clean
