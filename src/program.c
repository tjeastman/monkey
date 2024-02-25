#include <stdlib.h>

#include "monkey/program.h"
#include "monkey/statement.h"

Program* program_init()
{
    Program* program = (Program*)malloc(sizeof(Program));
    program->statements = (Statement**)malloc(sizeof(Statement*));
    program->size = 1;
    program->length = 0;
    return program;
}

void program_free(Program* program)
{
    for (size_t i = 0; i < program->length; ++i) {
        free(program->statements[i]);
    }
    free(program);
}

void program_extend(Program* program, Statement* statement)
{
    if (program->size <= program->length) {
        program->statements = (Statement**)realloc(program->statements, 2 * program->size * sizeof(Statement*));
        program->size *= 2;
    }
    program->statements[program->length] = statement;
    program->length++;
}

void program_print(Program* program)
{
    for (size_t i = 0; i < program->length; ++i) {
        statement_print(program->statements[i]);
    }
}
