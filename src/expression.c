#include <stdio.h>
#include <stdlib.h>

#include "monkey/expression.h"
#include "monkey/operation.h"

Expression* expression_init(ExpressionType type)
{
    Expression* expression = (Expression*)malloc(sizeof(Expression));
    expression->type = type;
    if (type == EXPRESSION_PREFIX) {
        expression->prefix.operand = NULL;
    } else if (type == EXPRESSION_INFIX) {
        expression->infix.operand[0] = NULL;
        expression->infix.operand[1] = NULL;
    }
    return expression;
}

void expression_free(Expression* expression)
{
    if (expression == NULL) {
        return;
    } else if (expression->type == EXPRESSION_IDENTIFIER) {
        free(expression->identifier.value);
    } else if (expression->type == EXPRESSION_STRING) {
        free(expression->string.value);
    } else if (expression->type == EXPRESSION_PREFIX) {
        expression_free(expression->prefix.operand);
    } else if (expression->type == EXPRESSION_INFIX) {
        expression_free(expression->infix.operand[0]);
        expression_free(expression->infix.operand[1]);
    }
    free(expression);
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
    printf("\"%s\"", expression.value);
}

void expression_print_identifier(IdentifierExpression expression)
{
    printf("%s", expression.value);
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

void expression_print(Expression* expression)
{
    switch (expression->type) {
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
    }
}
