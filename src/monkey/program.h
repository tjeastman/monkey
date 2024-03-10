#ifndef MONKEY_PROGRAM_H_
#define MONKEY_PROGRAM_H_

#include <stdlib.h>

#include "monkey/statement.h"

typedef struct Program Program;
struct Program {
    size_t size;
    size_t length;
    Statement* statements;
};

void program_init(Program*);
void program_free(Program*);
void program_extend(Program*, Statement);
void program_print(Program*);

#endif // MONKEY_PROGRAM_H_
