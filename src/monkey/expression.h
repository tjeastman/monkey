#ifndef MONKEY_EXPRESSION_H_
#define MONKEY_EXPRESSION_H_

#include <stdbool.h>

#include "monkey/operation.h"
#include "monkey/string.h"

typedef enum ExpressionType ExpressionType;
enum ExpressionType {
    EXPRESSION_NONE,
    EXPRESSION_INTEGER,
    EXPRESSION_BOOL,
    EXPRESSION_STRING,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_PREFIX,
    EXPRESSION_INFIX,
};

typedef struct IdentifierExpression IdentifierExpression;
struct IdentifierExpression {
    String value;
};

typedef struct BooleanExpression BooleanExpression;
struct BooleanExpression {
    bool value;
};

typedef struct IntegerExpression IntegerExpression;
struct IntegerExpression {
    int value;
};

typedef struct StringExpression StringExpression;
struct StringExpression {
    String value;
};

typedef struct Expression Expression;

typedef struct PrefixExpression PrefixExpression;
struct PrefixExpression {
    Operation operation;
    Expression* operand;
};

typedef struct InfixExpression InfixExpression;
struct InfixExpression {
    Operation operation;
    Expression* operand[2];
};

struct Expression {
    ExpressionType type;
    union {
        IntegerExpression integer;
        BooleanExpression boolean;
        StringExpression string;
        IdentifierExpression identifier;
        PrefixExpression prefix;
        InfixExpression infix;
    };
};

void expression_free(Expression*);
void expression_print(Expression*);

#endif // MONKEY_EXPRESSION_H_
