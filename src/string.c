#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/string.h"

void string_init(String* string)
{
    string->size = 0;
    string->position = 0;
    string->value = NULL;
}

void string_free(const String* string)
{
    if (string->value != NULL) {
        free(string->value);
    }
}

void string_reset(String* string)
{
    if (string->value != NULL) {
        memset(string->value, 0, string->size);
    }
    string->position = 0;
}

void string_append(String* string, char c)
{
    if (string->value == NULL) {
        string->value = (char*)malloc(2);
        memset(string->value, 0, 2);
        string->size = 2;
    } else if (string->position >= string->size - 1) {
        string->value = (char*)realloc(string->value, 2 * string->size);
        memset(string->value + string->size, 0, string->size);
        string->size *= 2;
    }
    string->value[string->position++] = c;
}

void string_copy(String* string, const String* source)
{
    string->size = source->size;
    string->position = source->position;
    if (source->value == NULL) {
        string->value = NULL;
    } else {
        string->value = (char*)malloc(source->size);
        memcpy(string->value, source->value, source->size);
    }
}

bool string_equal(const String* string, const String* string_alt)
{
    if (string->value == NULL && string_alt->value == NULL) {
        return true;
    } else if (string->value == NULL) {
        return false;
    } else if (string_alt->value == NULL) {
        return false;
    }
    return strcmp(string->value, string_alt->value) == 0;
}

void string_print(const String* string)
{
    if (string->value != NULL) {
        printf("%s", string->value);
    }
}
