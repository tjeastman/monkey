#include <stdio.h>

#include "monkey/expression.h"
#include "monkey/statement.h"
#include "monkey/string.h"

void statement_init(Statement* statement)
{
    statement->type = STATEMENT_NONE;
    statement->identifier = NULL;
    statement->expression.type = EXPRESSION_NONE;
}

void statement_free(Statement* statement)
{
    if (statement->identifier != NULL) {
        string_free(statement->identifier);
    }
    expression_free(&statement->expression);
}

void statement_print(Statement* statement)
{
    if (statement->type == STATEMENT_LET) {
        printf("let ");
        string_print(statement->identifier);
        printf(" = ");
    } else if (statement->type == STATEMENT_RETURN) {
        printf("return ");
    }
    expression_print(&statement->expression);
    printf(";\n");
}
