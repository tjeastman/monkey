#ifndef MONKEY_OBJECT_H_
#define MONKEY_OBJECT_H_

#include <stdbool.h>

#include "monkey/expression.h"
#include "monkey/string.h"

typedef enum ObjectType ObjectType;
enum ObjectType {
    OBJECT_NULL,
    OBJECT_INTEGER,
    OBJECT_STRING,
    OBJECT_BOOL,
    OBJECT_FUNCTION,
};

typedef struct Object Object;
struct Object {
    ObjectType type;
    union {
        int integer;
        char* string;
        bool boolean;
        FunctionExpression* function;
    };
    bool returned;
};

bool object_init_integer(Object*, int);
bool object_init_string(Object*, String*);
bool object_init_bool(Object*, bool);
bool object_init_function(Object*, FunctionExpression*);
void object_copy(Object*, const Object*);
void object_print(const Object*);

#endif // MONKEY_OBJECT_H_
