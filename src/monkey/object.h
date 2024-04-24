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
    OBJECT_ARRAY,
    OBJECT_FUNCTION,
    OBJECT_INTERNAL,
};

typedef struct ObjectArray ObjectArray;

typedef struct Object Object;
struct Object {
    ObjectType type;
    union {
        int integer;
        String* string;
        bool boolean;
        ObjectArray* objects;
        FunctionExpression* function;
        bool (*internal)(Object*);
    };
    bool returned;
};

struct ObjectArray {
    Object object;
    ObjectArray* next;
};

bool object_init_integer(Object*, int);
bool object_init_string(Object*, String*);
bool object_init_bool(Object*, bool);
bool object_init_array(Object*);
bool object_init_function(Object*, FunctionExpression*);
bool object_init_internal(Object*, bool (*)(Object*));
void object_free(Object*);
bool object_copy(Object*, const Object*);
bool object_equal(const Object*, const Object*);
void object_print(const Object*);

#endif // MONKEY_OBJECT_H_
