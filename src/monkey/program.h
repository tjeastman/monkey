#ifndef MONKEY_PROGRAM_H_
#define MONKEY_PROGRAM_H_

#include <stdlib.h>

#include "monkey/statement.h"

typedef struct {
    Statement** statements;
    size_t size;
    size_t length;
} Program;

Program* program_init();
void program_free(Program*);
void program_extend(Program*, Statement*);
void program_print(Program*);

#endif // MONKEY_PROGRAM_H_
