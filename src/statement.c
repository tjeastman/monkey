#include <stdio.h>

#include "monkey/expression.h"
#include "monkey/statement.h"
#include "monkey/string.h"

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

void statement_block_print(const StatementBlock* block)
{
    Statement* statement = block->head;
    while (statement != NULL) {
        statement_print(statement);
        statement = statement->next;
    }
}
