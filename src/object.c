#include <stdio.h>

#include "monkey/object.h"

void object_copy(Object* object, const Object* source)
{
    object->type = source->type;
    if (source->type == OBJECT_BOOL) {
        object->boolean = source->boolean;
    } else if (source->type == OBJECT_INTEGER) {
        object->integer = source->integer;
    } else if (source->type == OBJECT_STRING) {
        object->string = source->string;
    }
}

void object_print(const Object* object)
{
    switch (object->type) {
    case OBJECT_NULL:
        printf("NULL\n");
        break;
    case OBJECT_BOOL:
        printf("bool: %s\n", object->boolean ? "true" : "false");
        break;
    case OBJECT_INTEGER:
        printf("integer: %d\n", object->integer);
        break;
    case OBJECT_STRING:
        printf("string: %s\n", object->string);
        break;
    }
}
