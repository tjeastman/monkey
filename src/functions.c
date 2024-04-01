#include <stdbool.h>
#include <stdio.h>

#include "monkey/object.h"
#include "monkey/string.h"

bool function_length(Object* object)
{
    if (object->type != OBJECT_STRING) {
        printf("*** EVALUATION ERROR: expected string argument in len function\n");
        return false;
    }
    int length = string_length(object->string);
    object_free(object);
    object_init_integer(object, length);
    return true;
}

bool function_puts(Object* object)
{
    object_print(object);
    object_free(object);
    return true;
}
