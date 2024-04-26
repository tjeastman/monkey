#ifndef MONKEY_LIST_H_
#define MONKEY_LIST_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct List List;
struct List {
    List* next;
    void* data;
};

bool list_append(List**, void*, size_t);

#endif // MONKEY_LIST_H_
