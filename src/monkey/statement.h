#ifndef MONKEY_STATEMENT_H_
#define MONKEY_STATEMENT_H_

#include "monkey/expression.h"
#include "monkey/string.h"

typedef enum StatementType StatementType;
enum StatementType {
    STATEMENT_NONE,
    STATEMENT_LET,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
};

typedef struct Statement Statement;
struct Statement {
    StatementType type;
    String* identifier;
    Expression expression;
};

void statement_init(Statement*);
void statement_free(const Statement*);
void statement_print(const Statement*);

#endif // MONKEY_STATEMENT_H_
