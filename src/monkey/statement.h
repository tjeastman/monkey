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
    Statement* next;
};

typedef struct StatementBlock StatementBlock;
struct StatementBlock {
    Statement* head;
    Statement* tail;
};

bool statement_init_let(Statement*, String*);
bool statement_init_return(Statement*);
bool statement_init_expression(Statement*);
void statement_free(const Statement*);
void statement_print(const Statement*);
void statement_block_init(StatementBlock*);
void statement_block_free(const StatementBlock*);
void statement_block_extend(StatementBlock*, const Statement*);
void statement_block_print(const StatementBlock*);

#endif // MONKEY_STATEMENT_H_
