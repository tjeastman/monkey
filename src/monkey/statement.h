#ifndef MONKEY_STATEMENT_H_
#define MONKEY_STATEMENT_H_

#include "monkey/expression.h"

typedef enum {
    STATEMENT_LET,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
} StatementType;

typedef struct {
    StatementType type;
    char* identifier;
    Expression* expression;
} Statement;

Statement* statement_init(StatementType, char*, Expression*);
void statement_print(Statement*);
void statement_free(Statement*);

#endif // MONKEY_STATEMENT_H_
