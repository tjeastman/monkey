#ifndef MONKEY_EXPRESSION_H_
#define MONKEY_EXPRESSION_H_

#include <stdbool.h>

#include "monkey/operation.h"

typedef enum {
    EXPRESSION_INTEGER,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_STRING,
    EXPRESSION_BOOL,
    EXPRESSION_PREFIX,
    EXPRESSION_INFIX,
} ExpressionType;

typedef struct {
    char* value;
} IdentifierExpression;

typedef struct {
    bool value;
} BooleanExpression;

typedef struct {
    int value;
} IntegerExpression;

typedef struct {
    char* value;
} StringExpression;

typedef struct Expression Expression;

typedef struct {
    Operation operation;
    Expression* operand;
} PrefixExpression;

typedef struct {
    Operation operation;
    Expression* operand[2];
} InfixExpression;

struct Expression {
    ExpressionType type;
    union {
        IdentifierExpression identifier;
        BooleanExpression boolean;
        IntegerExpression integer;
        StringExpression string;
        PrefixExpression prefix;
        InfixExpression infix;
    };
};

Expression* expression_init(ExpressionType);
void expression_print(Expression*);
void expression_free(Expression*);

#endif // MONKEY_EXPRESSION_H_
