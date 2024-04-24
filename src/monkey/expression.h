#ifndef MONKEY_EXPRESSION_H_
#define MONKEY_EXPRESSION_H_

#include <stdbool.h>

#include "monkey/list.h"
#include "monkey/operation.h"
#include "monkey/string.h"

typedef struct StatementBlock StatementBlock;

typedef enum ExpressionType ExpressionType;
enum ExpressionType {
    EXPRESSION_NONE,
    EXPRESSION_INTEGER,
    EXPRESSION_BOOL,
    EXPRESSION_STRING,
    EXPRESSION_IDENTIFIER,
    EXPRESSION_PREFIX,
    EXPRESSION_INFIX,
    EXPRESSION_CONDITIONAL,
    EXPRESSION_FUNCTION,
    EXPRESSION_CALL,
    EXPRESSION_ARRAY,
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

typedef struct ConditionalExpression ConditionalExpression;
struct ConditionalExpression {
    Expression* condition;
    StatementBlock* consequence;
    StatementBlock* alternate;
};

typedef struct FunctionExpression FunctionExpression;
struct FunctionExpression {
    List* parameters;
    StatementBlock* body;
};

typedef struct CallExpression CallExpression;
struct CallExpression {
    List* arguments;
    Expression* function;
};

typedef struct ArrayExpression ArrayExpression;
struct ArrayExpression {
    Expression* expression;
    ArrayExpression* next;
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
        ConditionalExpression conditional;
        FunctionExpression function;
        CallExpression call;
        ArrayExpression* array;
    };
};

Expression* expression_new();
Expression* expression_move(const Expression*);
bool expression_init_integer(Expression*, int);
bool expression_init_bool(Expression*, bool);
bool expression_init_string(Expression*, String*);
bool expression_init_identifier(Expression*, String*);
bool expression_init_prefix(Expression*, Operation);
bool expression_init_infix(Expression*, Expression*, Operation);
bool expression_init_conditional(Expression*);
bool expression_init_function(Expression*);
bool expression_init_call(Expression*, Expression*);
bool expression_init_array(Expression*);
void expression_free(const Expression*);
void expression_print_function_parameters(List*);
void expression_print(const Expression*, int, bool);
bool array_element_new(ArrayExpression**);

#endif // MONKEY_EXPRESSION_H_
