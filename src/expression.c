#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/expression.h"
#include "monkey/operation.h"
#include "monkey/string.h"

void statement_block_free(const StatementBlock*);
void statement_block_print(const StatementBlock*);

Expression* expression_new()
{
    Expression* expression = (Expression*)malloc(sizeof(Expression));
    expression->type = EXPRESSION_NONE;
    return expression;
}

Expression* expression_move(const Expression* source)
{
    Expression* expression = (Expression*)malloc(sizeof(Expression));
    memcpy(expression, source, sizeof(Expression));
    return expression;
}

bool expression_init_integer(Expression* expression, int value)
{
    expression->type = EXPRESSION_INTEGER;
    expression->integer.value = value;
    return true;
}

bool expression_init_bool(Expression* expression, bool value)
{
    expression->type = EXPRESSION_BOOL;
    expression->boolean.value = value;
    return true;
}

bool expression_init_string(Expression* expression, String* value)
{
    expression->type = EXPRESSION_STRING;
    string_copy(&expression->string.value, value);
    return true;
}

bool expression_init_identifier(Expression* expression, String* value)
{
    expression->type = EXPRESSION_IDENTIFIER;
    string_copy(&expression->identifier.value, value);
    return true;
}

bool expression_init_prefix(Expression* expression, Operation operation)
{
    expression->type = EXPRESSION_PREFIX;
    expression->prefix.operation = operation;
    expression->prefix.operand = expression_new();
    return true;
}

bool expression_init_infix(Expression* expression, Expression* expression_left, Operation operation)
{
    expression->type = EXPRESSION_INFIX;
    expression->infix.operand[0] = expression_left;
    expression->infix.operand[1] = expression_new();
    expression->infix.operation = operation;
    return true;
}

bool expression_init_conditional(Expression* expression)
{
    expression->type = EXPRESSION_CONDITIONAL;
    expression->conditional.condition = expression_new();
    expression->conditional.consequence = NULL;
    expression->conditional.alternate = NULL;
    return true;
}

bool expression_init_function(Expression* expression)
{
    expression->type = EXPRESSION_FUNCTION;
    expression->function.parameters = NULL;
    expression->function.body = NULL;
    return true;
}

bool expression_init_call(Expression* expression, Expression* function)
{
    expression->type = EXPRESSION_CALL;
    expression->call.function = function;
    expression->call.arguments = NULL;
    return true;
}

bool expression_init_puts(Expression* expression)
{
    expression->type = EXPRESSION_PUTS;
    expression->puts.expression = expression_new();
    return true;
}

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

void expression_free_function_parameters(const FunctionParameter* parameter)
{
    string_free(&parameter->name);
    if (parameter->next != NULL) {
        expression_free_function_parameters(parameter->next);
        free(parameter->next);
    }
}

void expression_free_function(const FunctionExpression* expression)
{
    if (expression->parameters != NULL) {
        expression_free_function_parameters(expression->parameters);
        free(expression->parameters);
    }
    if (expression->body != NULL) {
        statement_block_free(expression->body);
        free(expression->body);
    }
}

void expression_free_call_arguments(const FunctionArgument* argument)
{
    expression_free(argument->expression);
    if (argument->next != NULL) {
        expression_free_call_arguments(argument->next);
        free(argument->next);
    }
}

void expression_free_call(const CallExpression* expression)
{
    expression_free(expression->function);
    if (expression->arguments != NULL) {
        expression_free_call_arguments(expression->arguments);
        free(expression->arguments);
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
    } else if (expression->type == EXPRESSION_FUNCTION) {
        expression_free_function(&expression->function);
    } else if (expression->type == EXPRESSION_CALL) {
        expression_free_call(&expression->call);
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

void expression_print_function_parameters(FunctionParameter* parameters)
{
    while (parameters != NULL) {
        string_print(&parameters->name);
        if (parameters->next != NULL) {
            printf(", ");
        }
        parameters = parameters->next;
    }
}

void expression_print_function(FunctionExpression expression)
{
    printf("fn(");
    expression_print_function_parameters(expression.parameters);
    printf(") {\n    ");
    statement_block_print(expression.body);
    putchar('\n');
    putchar('}');
}

void expression_print_call_arguments(FunctionArgument* arguments)
{
    while (arguments != NULL) {
        expression_print(arguments->expression);
        if (arguments->next != NULL) {
            printf(", ");
        }
        arguments = arguments->next;
    }
}

void expression_print_call(CallExpression expression)
{
    expression_print(expression.function);
    putchar('(');
    expression_print_call_arguments(expression.arguments);
    putchar(')');
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
    case EXPRESSION_FUNCTION:
        expression_print_function(expression->function);
        break;
    case EXPRESSION_CALL:
        expression_print_call(expression->call);
        break;
    case EXPRESSION_PUTS:
        expression_print_puts(expression->puts);
    }
}
