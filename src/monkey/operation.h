#ifndef MONKEY_OPERATION_H_
#define MONKEY_OPERATION_H_

typedef enum Operation Operation;
enum Operation {
    OPERATION_NONE,
    OPERATION_EQUAL,
    OPERATION_NOT_EQUAL,
    OPERATION_GREATER,
    OPERATION_GREATER_EQUAL,
    OPERATION_LESS,
    OPERATION_LESS_EQUAL,
    OPERATION_ADD,
    OPERATION_SUBTRACT,
    OPERATION_MULTIPLY,
    OPERATION_DIVIDE,
    OPERATION_NOT,
    OPERATION_NEGATIVE,
};

typedef enum Precedence Precedence;
enum Precedence {
    PRECEDENCE_LOWEST = 0,
    PRECEDENCE_EQUALITY = 1,
    PRECEDENCE_RELATIONAL = 2,
    PRECEDENCE_SUM = 3,
    PRECEDENCE_PRODUCT = 4,
    PRECEDENCE_PREFIX = 5,
    PRECEDENCE_CALL = 6,
};

Precedence operation_precedence(Operation);
void operation_print(Operation);

#endif // MONKEY_OPERATION_H_
