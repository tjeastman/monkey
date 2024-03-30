#include <stdio.h>
#include <stdlib.h>

#include "monkey/expression.h"
#include "monkey/operation.h"
#include "monkey/string.h"

void statement_block_free(const StatementBlock*);
void statement_block_print(const StatementBlock*);

void expression_free_prefix(const PrefixExpression* expression)
{
    expression_free(expression->operand);
    free(expression->operand);
}

void expression_free_infix(const InfixExpression* expression)
{
    expression_free(expression->operand[0]);
    free(expression->operand[0]);
    expression_free(expression->operand[1]);
    free(expression->operand[1]);
}

void expression_free_conditional(const ConditionalExpression* expression)
{
    expression_free(expression->condition);
    free(expression->condition);
    if (expression->consequence != NULL) {
        statement_block_free(expression->consequence);
        free(expression->consequence);
    }
    if (expression->alternate != NULL) {
        statement_block_free(expression->alternate);
        free(expression->alternate);
    }
}

void expression_free(const Expression* expression)
{
    if (expression->type == EXPRESSION_STRING) {
        string_free(&expression->string.value);
    } else if (expression->type == EXPRESSION_IDENTIFIER) {
        string_free(&expression->identifier.value);
    } else if (expression->type == EXPRESSION_PREFIX) {
        expression_free_prefix(&expression->prefix);
    } else if (expression->type == EXPRESSION_INFIX) {
        expression_free_infix(&expression->infix);
    } else if (expression->type == EXPRESSION_CONDITIONAL) {
        expression_free_conditional(&expression->conditional);
    }
}

void expression_print_bool(BooleanExpression expression)
{
    printf("%s", expression.value ? "true" : "false");
}

void expression_print_integer(IntegerExpression expression)
{
    printf("%d", expression.value);
}

void expression_print_string(StringExpression expression)
{
    putchar('"');
    string_print(&expression.value);
    putchar('"');
}

void expression_print_identifier(IdentifierExpression expression)
{
    string_print(&expression.value);
}

void expression_print_prefix(PrefixExpression expression)
{
    putchar('(');
    operation_print(expression.operation);
    expression_print(expression.operand);
    putchar(')');
}

void expression_print_infix(InfixExpression expression)
{
    putchar('(');
    expression_print(expression.operand[0]);
    operation_print(expression.operation);
    expression_print(expression.operand[1]);
    putchar(')');
}

void expression_print_conditional(ConditionalExpression expression)
{
    printf("if (");
    expression_print(expression.condition);
    printf(")");
    if (expression.consequence != NULL) {
        printf(" {\n    ");
        statement_block_print(expression.consequence);
        printf("}");
    }
    if (expression.alternate != NULL) {
        printf(" else {\n    ");
        statement_block_print(expression.alternate);
        printf("}");
    }
}

void expression_print_puts(PutsExpression expression)
{
    printf("puts(");
    expression_print(expression.expression);
    putchar(')');
}

void expression_print(const Expression* expression)
{
    switch (expression->type) {
    case EXPRESSION_NONE:
        break;
    case EXPRESSION_BOOL:
        expression_print_bool(expression->boolean);
        break;
    case EXPRESSION_INTEGER:
        expression_print_integer(expression->integer);
        break;
    case EXPRESSION_STRING:
        expression_print_string(expression->string);
        break;
    case EXPRESSION_IDENTIFIER:
        expression_print_identifier(expression->identifier);
        break;
    case EXPRESSION_PREFIX:
        expression_print_prefix(expression->prefix);
        break;
    case EXPRESSION_INFIX:
        expression_print_infix(expression->infix);
        break;
    case EXPRESSION_CONDITIONAL:
        expression_print_conditional(expression->conditional);
        break;
    case EXPRESSION_PUTS:
        expression_print_puts(expression->puts);
        break;
    }
}
