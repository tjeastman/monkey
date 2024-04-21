#ifndef MONKEY_ENVIRONMENT_H_
#define MONKEY_ENVIRONMENT_H_

#include <stdbool.h>

#include "monkey/hash.h"
#include "monkey/object.h"
#include "monkey/string.h"

typedef struct Environment Environment;
struct Environment {
    HashTable table;
    Environment* next;
};

void environment_init(Environment*, Environment*);
void environment_free(Environment*);
bool environment_insert(Environment*, const String*, const Object*);
bool environment_retrieve(const Environment*, const String*, Object*);

#endif // MONKEY_ENVIRONMENT_H_
