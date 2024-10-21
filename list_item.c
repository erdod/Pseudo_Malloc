#include <stdio.h>
#include <stdlib.h>
#include "list_item.h"

void List_init(ListHead* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void List_push_item(ListHead* list, ListItem* item) {
    item->next = list->head;
    item->prev = NULL;
    if (list->head) {
        list->head->prev = item;
    } else {
        list->tail = item; 
    }
    list->head = item;
    list->size++;
}

ListItem* List_remove_item(ListHead* list) {
    if (list->head == NULL) {
        return NULL; 
    }

    ListItem* item = list->head;
    list->head = item->next;
    if (list->head) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL; 
    }
    list->size--;
    return item;
}

int List_isEmpty(ListHead* list) {
    return list->size == 0;
}
