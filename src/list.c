#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/list.h"

bool list_append(List** list, void* data, size_t size)
{
    List* item = (List*)malloc(sizeof(List));
    item->next = NULL;
    item->data = malloc(size);
    memcpy(item->data, data, size);
    *list = item;
    return true;
}
