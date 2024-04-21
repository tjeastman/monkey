#include <stdbool.h>
#include <stdlib.h>

#include "monkey/environment.h"
#include "monkey/hash.h"
#include "monkey/object.h"

void environment_init(Environment* environment, Environment* environment_next)
{
    hash_init(&environment->table);
    environment->next = environment_next;
}

void environment_free(Environment* environment)
{
    hash_free(&environment->table);
}

bool environment_insert(Environment* environment, const String* string, const Object* source)
{
    Object* object = (Object*)hash_retrieve(&environment->table, string->value);
    if (object == NULL) {
        object = (Object*)malloc(sizeof(Object));
        object_copy(object, source);
        hash_insert(&environment->table, string->value, object);
    } else {
        object_copy(object, source);
    }
    return true;
}

bool environment_retrieve_aux(const Environment* environment, const String* string, Object* destination)
{
    Object* object = (Object*)hash_retrieve(&environment->table, string->value);
    if (object != NULL) {
        object_copy(destination, object);
        return true;
    }
    return false;
}

bool environment_retrieve(const Environment* environment, const String* string, Object* destination)
{
    for (; environment != NULL; environment = environment->next) {
        if (environment_retrieve_aux(environment, string, destination)) {
            return true;
        }
    }
    return false;
}
