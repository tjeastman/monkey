#include <stdbool.h>
#include <stdlib.h>

#include "monkey/environment.h"
#include "monkey/hash.h"
#include "monkey/object.h"

Environment* environment_push(const Environment* environment_next)
{
    Environment* environment = (Environment*)malloc(sizeof(Environment));
    environment->table = (HashTable*)malloc(sizeof(HashTable));
    hash_init(environment->table);
    environment->next = environment_next;
    return environment;
}

void environment_pop(const Environment* environment)
{
    hash_free(environment->table);
    free(environment->table);
}

void environment_insert(const Environment* environment, const String* string, const Object* source)
{
    Object* object = (Object*)hash_retrieve(environment->table, string->value);
    if (object == NULL) {
        object = (Object*)malloc(sizeof(Object));
        object_copy(object, source);
        hash_insert(environment->table, string->value, object);
    } else {
        object_copy(object, source);
    }
}

bool environment_retrieve_aux(const Environment* environment, const String* string, Object* destination)
{
    Object* object = (Object*)hash_retrieve(environment->table, string->value);
    if (object != NULL) {
        object_copy(destination, object);
        return true;
    }
    return false;
}

bool environment_retrieve(const Environment* environment, const String* string, Object* destination)
{
    while (environment != NULL) {
        if (environment_retrieve_aux(environment, string, destination)) {
            return true;
        }
        environment = environment->next;
    }
    return false;
}
