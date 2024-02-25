#include <stdio.h>
#include <stdlib.h>

#include "monkey/expression.h"
#include "monkey/statement.h"
#include "monkey/string.h"

Statement* statement_init(StatementType type, char* identifier, Expression* expression)
{
    Statement* statement = (Statement*)malloc(sizeof(Statement));
    statement->type = type;
    statement->identifier = identifier == NULL ? NULL : string_copy(identifier);
    statement->expression = expression;
    return statement;
}

void statement_print(Statement* statement)
{
    if (statement->type == STATEMENT_LET) {
        printf("let %s = ", statement->identifier);
    } else if (statement->type == STATEMENT_RETURN) {
        printf("return ");
    }
    expression_print(statement->expression);
    printf(";\n");
}

void statement_free(Statement* statement)
{
    if (statement->identifier != NULL) {
        free(statement->identifier);
    }
    if (statement->expression != NULL) {
        free(statement->expression);
    }
    free(statement);
}
