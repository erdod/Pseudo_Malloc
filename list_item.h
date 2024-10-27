#ifndef LIST_ITEM_H
#define LIST_ITEM_H

#include <stdio.h>
#include <stdlib.h>

typedef struct ListItem {
    struct ListItem* next;
    struct ListItem* prev; 
} ListItem;

typedef struct ListHead {
    ListItem* head; 
    ListItem* tail; 
    int size;       
} ListHead;

void List_init(ListHead* list);
void List_push_item(ListHead* list, ListItem* item);
ListItem* List_remove_item(ListHead* list);
int List_isEmpty(ListHead* list);
void List_print(ListHead* list);

#endif 
