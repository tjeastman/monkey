#ifndef MONKEY_OBJECT_H_
#define MONKEY_OBJECT_H_

#include <stdbool.h>

typedef enum ObjectType ObjectType;
enum ObjectType {
    OBJECT_NULL,
    OBJECT_INTEGER,
    OBJECT_STRING,
    OBJECT_BOOL,
};

typedef struct Object Object;
struct Object {
    ObjectType type;
    union {
        int integer;
        char* string;
        bool boolean;
    };
};

void object_copy(Object*, const Object*);
void object_print(const Object*);

#endif // MONKEY_OBJECT_H_
