#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/expression.h"
#include "monkey/object.h"
#include "monkey/program.h"
#include "monkey/statement.h"

bool evaluate_identifier_expression(IdentifierExpression* expression, Object* object)
{
    return false;
}

bool evaluate_integer_expression(IntegerExpression* expression, Object* object)
{
    object->type = OBJECT_INTEGER;
    object->integer = expression->value;
    return true;
}

bool evaluate_string_expression(StringExpression* expression, Object* object)
{
    object->type = OBJECT_STRING;
    object->string = expression->value.value;
    return true;
}

bool evaluate_bool_expression(BooleanExpression* expression, Object* object)
{
    object->type = OBJECT_BOOL;
    object->boolean = expression->value;
    return true;
}

bool evaluate_prefix_negative_operation(Object* object)
{
    if (object->type != OBJECT_INTEGER) {
        return false;
    }
    object->integer *= -1;
    return true;
}

bool evaluate_prefix_not_operation(Object* object)
{
    if (object->type != OBJECT_BOOL) {
        return false;
    }
    object->boolean = !object->boolean;
    return true;
}

bool evaluate_expression(Expression*, Object*);

bool evaluate_prefix_expression(PrefixExpression* expression, Object* object)
{
    if (!evaluate_expression(expression->operand, object)) {
        return false;
    } else if (expression->operation == OPERATION_NEGATIVE) {
        return evaluate_prefix_negative_operation(object);
    } else if (expression->operation == OPERATION_NOT) {
        return evaluate_prefix_not_operation(object);
    }
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
        return false;
    }

    object->type = OBJECT_BOOL;
    object->boolean = result;
    return true;
}

bool evaluate_infix_inequality_operation(Operation operation, Object* object, Object* object_right)
{
    if (object->type != OBJECT_INTEGER) {
        return false;
    } else if (object->type != object_right->type) {
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
        return false;
    }

    object->type = OBJECT_BOOL;
    object->boolean = result;
    return true;
}

bool evaluate_infix_arithmetic_operation(Operation operation, Object* object, Object* object_right)
{
    if (object->type != OBJECT_INTEGER) {
        return false;
    } else if (object->type != object_right->type) {
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

bool evaluate_infix_expression(InfixExpression* expression, Object* object)
{
    Object object_right;
    if (!evaluate_expression(expression->operand[1], &object_right)) {
        return false;
    } else if (!evaluate_expression(expression->operand[0], object)) {
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

bool evaluate_expression(Expression* expression, Object* object)
{
    switch (expression->type) {
    case EXPRESSION_IDENTIFIER:
        return evaluate_identifier_expression(&expression->identifier, object);
    case EXPRESSION_INTEGER:
        return evaluate_integer_expression(&expression->integer, object);
    case EXPRESSION_STRING:
        return evaluate_string_expression(&expression->string, object);
    case EXPRESSION_BOOL:
        return evaluate_bool_expression(&expression->boolean, object);
    case EXPRESSION_PREFIX:
        return evaluate_prefix_expression(&expression->prefix, object);
    case EXPRESSION_INFIX:
        return evaluate_infix_expression(&expression->infix, object);
    default:
        return false;
    }
}

void evaluate_expression_statement(Statement* statement)
{
    Object object;
    if (evaluate_expression(&statement->expression, &object)) {
        object_print(&object);
    } else {
        printf("ERROR\n");
    }
}

void evaluate_statement(Statement* statement)
{
    bool result;
    switch (statement->type) {
    case STATEMENT_LET:
        return;
    case STATEMENT_RETURN:
        return;
    case STATEMENT_EXPRESSION:
        evaluate_expression_statement(statement);
        return;
    default:
        printf("default\n");
    }
}

void evaluate_program(Program* program)
{
    for (size_t i = 0; i < program->length; ++i) {
        evaluate_statement(&program->statements[i]);
    }
}
