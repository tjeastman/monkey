#include <stdbool.h>
#include <stdio.h>

#include "monkey/expression.h"
#include "monkey/object.h"
#include "monkey/string.h"

bool object_init_integer(Object* object, int value)
{
    object->type = OBJECT_INTEGER;
    object->integer = value;
    return true;
}

bool object_init_string(Object* object, String* string)
{
    object->type = OBJECT_STRING;
    object->string = string->value;
    return true;
}

bool object_init_bool(Object* object, bool value)
{
    object->type = OBJECT_BOOL;
    object->boolean = value;
    return true;
}

bool object_init_function(Object* object, FunctionExpression* function)
{
    object->type = OBJECT_FUNCTION;
    object->function = function;
    return true;
}

void object_copy(Object* object, const Object* source)
{
    object->type = source->type;
    if (source->type == OBJECT_BOOL) {
        object->boolean = source->boolean;
    } else if (source->type == OBJECT_INTEGER) {
        object->integer = source->integer;
    } else if (source->type == OBJECT_STRING) {
        object->string = source->string;
    } else if (source->type == OBJECT_FUNCTION) {
        object->function = source->function;
    }
    object->returned = false;
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
    case OBJECT_FUNCTION:
        printf("function\n");
        break;
    }
}
