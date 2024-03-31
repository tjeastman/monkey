#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/environment.h"
#include "monkey/expression.h"
#include "monkey/object.h"
#include "monkey/program.h"
#include "monkey/statement.h"

bool evaluate_identifier_expression(Environment* environment, IdentifierExpression* expression, Object* object)
{
    if (!environment_retrieve(environment, &expression->value, object)) {
        printf("*** EVALUATION ERROR: missing variable: %s\n", expression->value.value);
        return false;
    }
    return true;
}

bool evaluate_integer_expression(Environment* environment, IntegerExpression* expression, Object* object)
{
    object->type = OBJECT_INTEGER;
    object->integer = expression->value;
    return true;
}

bool evaluate_string_expression(Environment* environment, StringExpression* expression, Object* object)
{
    object->type = OBJECT_STRING;
    object->string = expression->value.value;
    return true;
}

bool evaluate_bool_expression(Environment* environment, BooleanExpression* expression, Object* object)
{
    object->type = OBJECT_BOOL;
    object->boolean = expression->value;
    return true;
}

bool evaluate_prefix_negative_operation(Object* object)
{
    if (object->type != OBJECT_INTEGER) {
        printf("*** EVALUATION ERROR: unary - operation applied to non-integer value\n");
        return false;
    }
    object->integer *= -1;
    return true;
}

bool evaluate_prefix_not_operation(Object* object)
{
    if (object->type != OBJECT_BOOL) {
        printf("*** EVALUATION ERROR: unary ! operation applied to non-bool value\n");
        return false;
    }
    object->boolean = !object->boolean;
    return true;
}

bool evaluate_expression(Environment*, Expression*, Object*);

bool evaluate_prefix_expression(Environment* environment, PrefixExpression* expression, Object* object)
{
    if (!evaluate_expression(environment, expression->operand, object)) {
        return false;
    } else if (expression->operation == OPERATION_NEGATIVE) {
        return evaluate_prefix_negative_operation(object);
    } else if (expression->operation == OPERATION_NOT) {
        return evaluate_prefix_not_operation(object);
    }
    printf("*** EVALUATION ERROR: unrecognized prefix operation: ");
    operation_print(expression->operation);
    putchar('\n');
    return false;
}

bool evaluate_infix_comparison_operation(Operation operation, Object* object, Object* object_right)
{
    bool result;
    if (object->type != object_right->type) {
        result = false;
    } else if (object->type == OBJECT_INTEGER) {
        result = object->integer == object_right->integer;
    } else if (object->type == OBJECT_BOOL) {
        result = object->boolean == object_right->boolean;
    } else if (object->type == OBJECT_STRING) {
        result = strcmp(object->string, object_right->string) == 0;
    } else {
        return false;
    }

    if (operation == OPERATION_NOT_EQUAL) {
        result = !result;
    } else if (operation != OPERATION_EQUAL) {
        printf("*** EVALUATION ERROR: unrecognized infix comparison operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    }

    object->type = OBJECT_BOOL;
    object->boolean = result;
    return true;
}

bool evaluate_infix_inequality_operation(Operation operation, Object* object, Object* object_right)
{
    if (object->type != OBJECT_INTEGER) {
        printf("*** EVALUATION ERROR: expected integer type for infix operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    } else if (object->type != object_right->type) {
        printf("*** EVALUATION ERROR: mismatched types for infix operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    }

    bool result;
    if (operation == OPERATION_GREATER) {
        result = object->integer > object_right->integer;
    } else if (operation == OPERATION_GREATER_EQUAL) {
        result = object->integer >= object_right->integer;
    } else if (operation == OPERATION_LESS) {
        result = object->integer < object_right->integer;
    } else if (operation == OPERATION_LESS_EQUAL) {
        result = object->integer <= object_right->integer;
    } else {
        printf("*** EVALUATION ERROR: unrecognized infix inequality operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    }

    object->type = OBJECT_BOOL;
    object->boolean = result;
    return true;
}

bool evaluate_infix_arithmetic_operation(Operation operation, Object* object, Object* object_right)
{
    if (object->type != OBJECT_INTEGER) {
        printf("*** EVALUATION ERROR: expected integer type for infix operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    } else if (object->type != object_right->type) {
        printf("*** EVALUATION ERROR: mismatched types for infix operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    } else if (operation == OPERATION_ADD) {
        object->integer += object_right->integer;
    } else if (operation == OPERATION_SUBTRACT) {
        object->integer -= object_right->integer;
    } else if (operation == OPERATION_MULTIPLY) {
        object->integer *= object_right->integer;
    } else if (operation == OPERATION_DIVIDE) {
        object->integer /= object_right->integer;
    } else {
        return false;
    }
    return true;
}

bool evaluate_infix_expression(Environment* environment, InfixExpression* expression, Object* object)
{
    Object object_right;
    if (!evaluate_expression(environment, expression->operand[1], &object_right)) {
        return false;
    } else if (!evaluate_expression(environment, expression->operand[0], object)) {
        return false;
    }

    switch (expression->operation) {
    case OPERATION_EQUAL:
    case OPERATION_NOT_EQUAL:
        return evaluate_infix_comparison_operation(expression->operation, object, &object_right);
    case OPERATION_GREATER:
    case OPERATION_GREATER_EQUAL:
    case OPERATION_LESS:
    case OPERATION_LESS_EQUAL:
        return evaluate_infix_inequality_operation(expression->operation, object, &object_right);
    case OPERATION_ADD:
    case OPERATION_SUBTRACT:
    case OPERATION_MULTIPLY:
    case OPERATION_DIVIDE:
        return evaluate_infix_arithmetic_operation(expression->operation, object, &object_right);
    default:
        return false;
    }
}

bool evaluate_conditional_expression(Environment* environment, ConditionalExpression* expression, Object* object)
{
    if (!evaluate_expression(environment, expression->condition, object)) {
        return false;
    } else if (object->type != OBJECT_BOOL) {
        printf("*** EVALUATION ERROR: non-bool result in conditional expression\n");
        return false;
    }

    bool result = object->boolean;
    if (!result && expression->alternate == NULL) {
        object->type = OBJECT_NULL;
        return true;
    } else if (!result && !evaluate_expression(environment, expression->alternate, object)) {
        return false;
    } else if (result && !evaluate_expression(environment, expression->consequence, object)) {
        return false;
    }
    return true;
}

bool evaluate_puts_expression(Environment* environment, PutsExpression* expression, Object* object)
{
    if (!evaluate_expression(environment, expression->expression, object)) {
        return false;
    }
    object_print(object);
    object->type = OBJECT_NULL;
    return true;
}

bool evaluate_expression(Environment* environment, Expression* expression, Object* object)
{
    switch (expression->type) {
    case EXPRESSION_IDENTIFIER:
        return evaluate_identifier_expression(environment, &expression->identifier, object);
    case EXPRESSION_INTEGER:
        return evaluate_integer_expression(environment, &expression->integer, object);
    case EXPRESSION_STRING:
        return evaluate_string_expression(environment, &expression->string, object);
    case EXPRESSION_BOOL:
        return evaluate_bool_expression(environment, &expression->boolean, object);
    case EXPRESSION_PREFIX:
        return evaluate_prefix_expression(environment, &expression->prefix, object);
    case EXPRESSION_INFIX:
        return evaluate_infix_expression(environment, &expression->infix, object);
    case EXPRESSION_CONDITIONAL:
        return evaluate_conditional_expression(environment, &expression->conditional, object);
    case EXPRESSION_PUTS:
        return evaluate_puts_expression(environment, &expression->puts, object);
    default:
        printf("*** EVALUATION ERROR: unexpected expression type\n");
        return false;
    }
}

void evaluate_expression_statement(Environment* environment, Statement* statement)
{
    Object object;
    evaluate_expression(environment, &statement->expression, &object);
}

void evaluate_let_statement(Environment* environment, Statement* statement)
{
    Object object;
    if (evaluate_expression(environment, &statement->expression, &object)) {
        environment_insert(environment, statement->identifier, &object);
    }
}

void evaluate_statement(Environment* environment, Statement* statement)
{
    bool result;
    switch (statement->type) {
    case STATEMENT_LET:
        evaluate_let_statement(environment, statement);
        return;
    case STATEMENT_RETURN:
        return;
    case STATEMENT_EXPRESSION:
        evaluate_expression_statement(environment, statement);
        return;
    case STATEMENT_NONE:
        printf("*** EVALUATION ERROR: unexpected statement type\n");
    }
}

void evaluate_program(Program* program)
{
    Environment environment;
    environment_init(&environment);
    for (size_t i = 0; i < program->length; ++i) {
        evaluate_statement(&environment, &program->statements[i]);
    }
    environment_free(&environment);
}
