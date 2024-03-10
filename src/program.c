#include <stdlib.h>

#include "monkey/program.h"
#include "monkey/statement.h"

void program_init(Program* program)
{
    program->size = 1;
    program->length = 0;
    program->statements = (Statement*)malloc(sizeof(Statement));
}

void program_free(Program* program)
{
    for (size_t i = 0; i < program->length; ++i) {
        statement_free(&program->statements[i]);
    }
    free(program->statements);
}

void program_extend(Program* program, Statement statement)
{
    if (program->size <= program->length) {
        program->statements = (Statement*)realloc(program->statements, 2 * program->size * sizeof(Statement));
        program->size *= 2;
    }
    program->statements[program->length++] = statement;
}

void program_print(Program* program)
{
    for (size_t i = 0; i < program->length; ++i) {
        statement_print(&program->statements[i]);
    }
}
