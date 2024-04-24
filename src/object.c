#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/expression.h"
#include "monkey/object.h"
#include "monkey/string.h"

bool object_init_integer(Object* object, int value)
{
    object->type = OBJECT_INTEGER;
    object->integer = value;
    object->returned = false;
    return true;
}

bool object_init_string(Object* object, String* string)
{
    object->type = OBJECT_STRING;
    object->string = (String*)malloc(sizeof(String));
    string_copy(object->string, string);
    object->returned = false;
    return true;
}

bool object_init_bool(Object* object, bool value)
{
    object->type = OBJECT_BOOL;
    object->boolean = value;
    object->returned = false;
    return true;
}

bool object_init_array(Object* object)
{
    object->type = OBJECT_ARRAY;
    object->objects = NULL;
    object->returned = false;
    return true;
}

bool object_init_function(Object* object, FunctionExpression* function)
{
    object->type = OBJECT_FUNCTION;
    object->function = function;
    object->returned = false;
    return true;
}

bool object_init_internal(Object* object, bool (*internal)(Object*))
{
    object->type = OBJECT_INTERNAL;
    object->internal = internal;
    object->returned = false;
    return true;
}

void object_free_array(ObjectArray* objects)
{
    if (objects == NULL) {
        return;
    }
    object_free_array(objects->next);
    object_free(&objects->object);
    // FIX:
    // free(object);
}

void object_free(Object* object)
{
    if (object->type == OBJECT_STRING) {
        string_free(object->string);
        free(object->string);
    } else if (object->type == OBJECT_ARRAY) {
        object_free_array(object->objects);
    }
    object->type = OBJECT_NULL;
    object->returned = false;
}

bool object_copy_array(Object* object, const Object* source)
{
    if (!object_init_array(object)) {
        return false;
    }

    object->objects = (ObjectArray*)malloc(sizeof(ObjectArray));
    object->objects->next = NULL;
    ObjectArray* tmp = object->objects;

    for (ObjectArray* array = source->objects; array != NULL; array = array->next) {
        object_copy(&tmp->object, &array->object);
        if (array->next != NULL) {
            tmp->next = (ObjectArray*)malloc(sizeof(ObjectArray));
            tmp->next->next = NULL;
            tmp = tmp->next;
        }
    }

    return true;
}

bool object_copy(Object* object, const Object* source)
{
    if (source->type == OBJECT_NULL) {
        object->type = OBJECT_NULL;
        object->returned = false;
    } else if (source->type == OBJECT_BOOL) {
        return object_init_bool(object, source->boolean);
    } else if (source->type == OBJECT_INTEGER) {
        return object_init_integer(object, source->integer);
    } else if (source->type == OBJECT_STRING) {
        return object_init_string(object, source->string);
    } else if (source->type == OBJECT_ARRAY) {
        return object_copy_array(object, source);
    } else if (source->type == OBJECT_FUNCTION) {
        return object_init_function(object, source->function);
    } else if (source->type == OBJECT_INTERNAL) {
        return object_init_internal(object, source->internal);
    }
    return true;
}

bool object_equal_array(const ObjectArray* objects, const ObjectArray* objects_alt)
{
    if (objects == NULL && objects_alt == NULL) {
        return true;
    } else if (objects == NULL) {
        return false;
    } else if (objects_alt == NULL) {
        return false;
    } else if (!object_equal(&objects->object, &objects_alt->object)) {
        return false;
    }
    return object_equal_array(objects->next, objects_alt->next);
}

bool object_equal(const Object* object, const Object* object_alt)
{
    if (object->type != object_alt->type) {
        return false;
    }

    switch (object->type) {
    case OBJECT_NULL:
        return true;
    case OBJECT_BOOL:
        return object->boolean == object_alt->boolean;
    case OBJECT_INTEGER:
        return object->integer == object_alt->integer;
    case OBJECT_STRING:
        return string_equal(object->string, object_alt->string);
    case OBJECT_ARRAY:
        return object_equal_array(object->objects, object_alt->objects);
    case OBJECT_FUNCTION:
        return object->function == object_alt->function;
    case OBJECT_INTERNAL:
        return object->internal == object_alt->internal;
    }

    return false;
}

void object_print_array(ObjectArray* objects)
{
    if (objects == NULL) {
        return;
    }
    printf("  ");
    object_print(&objects->object);
    object_print_array(objects->next);
}

void object_print_function(FunctionExpression* function)
{
    printf("fn(");
    expression_print_function_parameters(function->parameters);
    printf(") {...}");
}

void object_print(const Object* object)
{
    switch (object->type) {
    case OBJECT_NULL:
        printf("NULL");
        break;
    case OBJECT_BOOL:
        printf("%s", object->boolean ? "true" : "false");
        break;
    case OBJECT_INTEGER:
        printf("%d", object->integer);
        break;
    case OBJECT_STRING:
        string_print(object->string);
        break;
    case OBJECT_ARRAY:
        printf("[\n");
        object_print_array(object->objects);
        printf("]\n");
        break;
    case OBJECT_FUNCTION:
        object_print_function(object->function);
        break;
    case OBJECT_INTERNAL:
        printf("internal");
        break;
    }
}
