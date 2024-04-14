#include <stdio.h>

#include "monkey/expression.h"
#include "monkey/statement.h"
#include "monkey/string.h"

Statement* statement_new()
{
    Statement* statement = (Statement*)malloc(sizeof(Statement));
    statement_init(statement);
    return statement;
}

void statement_init(Statement* statement)
{
    statement->type = STATEMENT_NONE;
    statement->identifier = NULL;
    statement->expression.type = EXPRESSION_NONE;
    statement->next = NULL;
}

void statement_free(const Statement* statement)
{
    if (statement->identifier != NULL) {
        string_free(statement->identifier);
    }
    expression_free(&statement->expression);
    if (statement->next != NULL) {
        statement_free(statement->next);
        free(statement->next);
    }
}

void statement_print(const Statement* statement)
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
