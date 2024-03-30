#ifndef MONKEY_ENVIRONMENT_H_
#define MONKEY_ENVIRONMENT_H_

#include <stdbool.h>

#include "monkey/hash.h"
#include "monkey/object.h"
#include "monkey/string.h"

typedef struct Environment Environment;
struct Environment {
    HashTable* table;
    const Environment* next;
};

Environment* environment_push(const Environment*);
void environment_pop(const Environment*);
void environment_insert(const Environment*, const String*, const Object*);
bool environment_retrieve(const Environment*, const String*, Object*);

#endif // MONKEY_ENVIRONMENT_H_
