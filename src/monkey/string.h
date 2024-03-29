#ifndef MONKEY_STRING_H_
#define MONKEY_STRING_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct String String;
struct String {
    size_t size;
    size_t position;
    char* value;
};

void string_init(String*);
void string_free(const String*);
void string_reset(String*);
bool string_append(String*, char);
bool string_concatenate(String*, const String*);
bool string_copy(String*, const String*);
bool string_equal(const String*, const String*);
void string_print(const String*);

#endif // MONKEY_STRING_H_
