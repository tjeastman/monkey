#include <stdio.h>
#include <stdlib.h>

#include "monkey/environment.h"
#include "monkey/eval.h"
#include "monkey/expression.h"
#include "monkey/functions.h"
#include "monkey/hash.h"
#include "monkey/object.h"
#include "monkey/statement.h"

bool evaluate_identifier_expression(Environment* environment, IdentifierExpression* expression, Object* object)
{
    if (!environment_retrieve(environment, &expression->value, object)) {
        printf("*** EVALUATION ERROR: missing variable: %s\n", expression->value.value);
        return false;
    }
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
    bool result = object_equal(object, object_right);

    if (operation == OPERATION_NOT_EQUAL) {
        result = !result;
    } else if (operation != OPERATION_EQUAL) {
        printf("*** EVALUATION ERROR: unrecognized infix comparison operation: ");
        operation_print(operation);
        putchar('\n');
        return false;
    }

    object_free(object);
    return object_init_bool(object, result);
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

    object_free(object);
    return object_init_bool(object, result);
}

bool evaluate_infix_arithmetic_operation(Operation operation, Object* object, Object* object_right)
{
    if (object->type == OBJECT_STRING) {
        if (operation != OPERATION_ADD) {
            printf("*** EVALUATION ERROR: invalid infix operation for string: ");
            operation_print(operation);
            putchar('\n');
            return false;
        } else if (object->type != object_right->type) {
            printf("*** EVALUATION ERROR: mismatched types for infix string operation: ");
            operation_print(operation);
            putchar('\n');
            return false;
        }
        return string_concatenate(object->string, object_right->string);
    }

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

bool evaluate_infix_expression_aux(Environment* environment, InfixExpression* expression, Object* object, Object* object_right)
{
    switch (expression->operation) {
    case OPERATION_EQUAL:
    case OPERATION_NOT_EQUAL:
        return evaluate_infix_comparison_operation(expression->operation, object, object_right);
    case OPERATION_GREATER:
    case OPERATION_GREATER_EQUAL:
    case OPERATION_LESS:
    case OPERATION_LESS_EQUAL:
        return evaluate_infix_inequality_operation(expression->operation, object, object_right);
    case OPERATION_ADD:
    case OPERATION_SUBTRACT:
    case OPERATION_MULTIPLY:
    case OPERATION_DIVIDE:
        return evaluate_infix_arithmetic_operation(expression->operation, object, object_right);
    default:
        return false;
    }
}

bool evaluate_infix_expression(Environment* environment, InfixExpression* expression, Object* object)
{
    if (!evaluate_expression(environment, expression->operand[0], object)) {
        return false;
    }

    Object object_right;
    if (!evaluate_expression(environment, expression->operand[1], &object_right)) {
        return false;
    }

    bool result = evaluate_infix_expression_aux(environment, expression, object, &object_right);
    object_free(&object_right);
    return result;
}

bool evaluate_statement_block_aux(Environment* environment, StatementBlock* block, Object* object)
{
    for (Statement* statement = block->head; statement != NULL; statement = statement->next) {
        if (!evaluate_statement(environment, statement, object)) {
            return false;
        } else if (object->returned) {
            break;
        } else if (statement->next != NULL) {
            object_free(object);
        }
    }
    return true;
}

bool evaluate_statement_block(Environment* environment, StatementBlock* block, Object* object)
{
    Environment environment_new;
    environment_init(&environment_new, environment);
    bool result = evaluate_statement_block_aux(&environment_new, block, object);
    environment_free(&environment_new);
    return result;
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
    object_free(object);
    if (!result && expression->alternate == NULL) {
        return true;
    } else if (!result && !evaluate_statement_block(environment, expression->alternate, object)) {
        return false;
    } else if (result && !evaluate_statement_block(environment, expression->consequence, object)) {
        return false;
    }
    return true;
}

bool evaluate_call_expression_arguments(Environment* environment, List* parameter, List* argument)
{
    if (parameter == NULL && argument == NULL) {
        return true;
    } else if (parameter == NULL) {
        printf("*** EVALUATION ERROR: too many arguments in call expression\n");
        return false;
    } else if (argument == NULL) {
        printf("*** EVALUATION ERROR: not enough arguments in call expression\n");
        return false;
    }

    // note need to use "previous" environment
    Object object;
    if (!evaluate_expression(environment->next, (Expression*)argument->data, &object)) {
        return false;
    } else if (!environment_insert(environment, (String*)parameter->data, &object)) {
        return false;
    }
    object_free(&object);

    return evaluate_call_expression_arguments(environment, parameter->next, argument->next);
}

bool evaluate_call_expression_external(Environment* environment, CallExpression* expression, Object* object_fn, Object* object)
{

    Environment environment_new;
    environment_init(&environment_new, environment);
    if (!evaluate_call_expression_arguments(&environment_new, object_fn->function->parameters, expression->arguments)) {
        environment_free(&environment_new);
        return false;
    } else if (!evaluate_statement_block_aux(&environment_new, object_fn->function->body, object)) {
        environment_free(&environment_new);
        return false;
    } else if (object->returned) {
        object->returned = false;
    }

    environment_free(&environment_new);
    return true;
}

bool evaluate_call_expression_internal(Environment* environment, CallExpression* expression, Object* object_fn, Object* object)
{
    if (expression->arguments == NULL) {
        printf("*** EVALUATION ERROR: not enough arguments in call expression\n");
        return false;
    } else if (expression->arguments->next != NULL) {
        printf("*** EVALUATION ERROR: too many arguments in call expression\n");
        return false;
    } else if (!evaluate_expression(environment, (Expression*)expression->arguments->data, object)) {
        return false;
    }

    return object_fn->internal(object);
}

bool evaluate_call_expression(Environment* environment, CallExpression* expression, Object* object)
{
    Object object_fn;
    bool result = false;
    if (!evaluate_expression(environment, expression->function, &object_fn)) {
        return false;
    } else if (object_fn.type == OBJECT_FUNCTION) {
        result = evaluate_call_expression_external(environment, expression, &object_fn, object);
    } else if (object_fn.type == OBJECT_INTERNAL) {
        result = evaluate_call_expression_internal(environment, expression, &object_fn, object);
    } else {
        printf("*** EVALUATION ERROR: non-function object in call expression\n");
    }
    object_free(&object_fn);
    return result;
}

bool evaluate_array_expression(Environment* environment, ArrayExpression* expression, Object* object)
{
    object_init_array(object);

    ObjectArray* element = (ObjectArray*)malloc(sizeof(ObjectArray));
    element->next = NULL;
    element->object.type = OBJECT_NULL;

    object->objects = element;

    for (; expression != NULL; expression = expression->next) {
        if (!evaluate_expression(environment, expression->expression, &element->object)) {
            printf("*** fail\n");
            object_free(object);
            return false;
        }
        if (expression->next != NULL) {
            element->next = (ObjectArray*)malloc(sizeof(ObjectArray));
            element = element->next;
            element->next = NULL;
            element->object.type = OBJECT_NULL;
        }
    }

    return true;
}

bool evaluate_expression(Environment* environment, Expression* expression, Object* object)
{
    switch (expression->type) {
    case EXPRESSION_IDENTIFIER:
        return evaluate_identifier_expression(environment, &expression->identifier, object);
    case EXPRESSION_INTEGER:
        return object_init_integer(object, expression->integer.value);
    case EXPRESSION_STRING:
        return object_init_string(object, &expression->string.value);
    case EXPRESSION_BOOL:
        return object_init_bool(object, expression->boolean.value);
    case EXPRESSION_PREFIX:
        return evaluate_prefix_expression(environment, &expression->prefix, object);
    case EXPRESSION_INFIX:
        return evaluate_infix_expression(environment, &expression->infix, object);
    case EXPRESSION_CONDITIONAL:
        return evaluate_conditional_expression(environment, &expression->conditional, object);
    case EXPRESSION_FUNCTION:
        return object_init_function(object, &expression->function);
    case EXPRESSION_CALL:
        return evaluate_call_expression(environment, &expression->call, object);
    case EXPRESSION_ARRAY:
        return evaluate_array_expression(environment, expression->array, object);
    default:
        printf("*** EVALUATION ERROR: unexpected expression type\n");
        return false;
    }
}

bool evaluate_let_statement(Environment* environment, Statement* statement, Object* object)
{
    if (!evaluate_expression(environment, &statement->expression, object)) {
        return false;
    } else if (!environment_insert(environment, statement->identifier, object)) {
        return false;
    }
    object_free(object);
    return true;
}

bool evaluate_return_statement(Environment* environment, Statement* statement, Object* object)
{
    if (!evaluate_expression(environment, &statement->expression, object)) {
        return false;
    }
    object->returned = true;
    return true;
}

bool evaluate_statement(Environment* environment, Statement* statement, Object* object)
{
    switch (statement->type) {
    case STATEMENT_LET:
        return evaluate_let_statement(environment, statement, object);
    case STATEMENT_RETURN:
        return evaluate_return_statement(environment, statement, object);
    case STATEMENT_EXPRESSION:
        return evaluate_expression(environment, &statement->expression, object);
    case STATEMENT_NONE:
        printf("*** EVALUATION ERROR: unexpected statement type\n");
        return false;
    }
}

void evaluate_program(StatementBlock* block)
{
    Environment environment;
    environment_init(&environment, NULL);

    Object* object;
    object = malloc(sizeof(Object));
    object_init_internal(object, function_puts);
    hash_insert(&environment.table, "puts", object);

    object = malloc(sizeof(Object));
    object_init_internal(object, function_length);
    hash_insert(&environment.table, "len", object);

    object = malloc(sizeof(Object));
    if (evaluate_statement_block_aux(&environment, block, object)) {
        object_free(object);
    }
    free(object);
    environment_free(&environment);
}
