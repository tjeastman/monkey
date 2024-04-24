#include <stdio.h>

#include "monkey/expression.h"
#include "monkey/statement.h"
#include "monkey/string.h"

bool statement_init_let(Statement* statement, String* identifier)
{
    statement->type = STATEMENT_LET;
    statement->identifier = (String*)malloc(sizeof(String));
    string_copy(statement->identifier, identifier);
    statement->expression.type = EXPRESSION_NONE;
    statement->next = NULL;
    return true;
}

bool statement_init_return(Statement* statement)
{
    statement->type = STATEMENT_RETURN;
    statement->identifier = NULL;
    statement->expression.type = EXPRESSION_NONE;
    statement->next = NULL;
    return true;
}

bool statement_init_expression(Statement* statement)
{
    statement->type = STATEMENT_EXPRESSION;
    statement->identifier = NULL;
    statement->expression.type = EXPRESSION_NONE;
    statement->next = NULL;
    return true;
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

void statement_print(const Statement* statement, int indent)
{
    if (statement->type == STATEMENT_LET) {
        printf("%*s", indent * 4, "");
        printf("let ");
        string_print(statement->identifier);
        printf(" = ");
        expression_print(&statement->expression, 0, false);
    } else if (statement->type == STATEMENT_RETURN) {
        printf("%*s", indent * 4, "");
        printf("return ");
        expression_print(&statement->expression, 0, false);
    } else {
        expression_print(&statement->expression, indent, false);
    }
    printf(";\n");
}

void statement_block_init(StatementBlock* block)
{
    block->head = NULL;
    block->tail = NULL;
}

void statement_block_free(const StatementBlock* block)
{
    if (block->head != NULL) {
        statement_free(block->head);
        free(block->head);
    }
}

void statement_block_extend(StatementBlock* block, const Statement* statement)
{
    if (block->head == NULL) {
        block->head = (Statement*)malloc(sizeof(Statement));
        *block->head = *statement;
        block->tail = block->head;
    } else {
        block->tail->next = (Statement*)malloc(sizeof(Statement));
        *block->tail->next = *statement;
        block->tail = block->tail->next;
    }
}

void statement_block_print(const StatementBlock* block, int indent)
{
    Statement* statement = block->head;
    while (statement != NULL) {
        statement_print(statement, indent);
        statement = statement->next;
    }
}
