#include <stdio.h>

#include "monkey/object.h"

void object_print(Object* object)
{
    switch (object->type) {
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
