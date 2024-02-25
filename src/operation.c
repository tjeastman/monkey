#include <stdio.h>

#include "monkey/operation.h"

Precedence operation_precedence(Operation operation)
{
    switch (operation) {
    case OPERATION_EQUAL:
    case OPERATION_NOT_EQUAL:
        return PRECEDENCE_EQUALITY;
    case OPERATION_GREATER:
    case OPERATION_GREATER_EQUAL:
    case OPERATION_LESS:
    case OPERATION_LESS_EQUAL:
        return PRECEDENCE_RELATIONAL;
    case OPERATION_ADD:
    case OPERATION_SUBTRACT:
        return PRECEDENCE_SUM;
    case OPERATION_MULTIPLY:
    case OPERATION_DIVIDE:
        return PRECEDENCE_PRODUCT;
    case OPERATION_NOT:
    case OPERATION_NEGATIVE:
        return PRECEDENCE_PREFIX;
    }
}

void operation_print(Operation operation)
{
    switch (operation) {
    case OPERATION_EQUAL:
        printf(" == ");
        break;
    case OPERATION_NOT_EQUAL:
        printf(" != ");
        break;
    case OPERATION_GREATER:
        printf(" > ");
        break;
    case OPERATION_GREATER_EQUAL:
        printf(" >= ");
        break;
    case OPERATION_LESS:
        printf(" < ");
        break;
    case OPERATION_LESS_EQUAL:
        printf(" <= ");
        break;
    case OPERATION_ADD:
        printf(" + ");
        break;
    case OPERATION_SUBTRACT:
        printf(" - ");
        break;
    case OPERATION_MULTIPLY:
        printf(" * ");
        break;
    case OPERATION_DIVIDE:
        printf(" / ");
        break;
    case OPERATION_NOT:
        printf("!");
        break;
    case OPERATION_NEGATIVE:
        printf("-");
        break;
    }
}
