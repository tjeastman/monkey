#ifndef MONKEY_ERROR_H_
#define MONKEY_ERROR_H_

#include <stddef.h>

#include "monkey/token.h"

typedef enum ErrorType ErrorType;
enum ErrorType {
    ERROR_LET_TOKEN_ASSIGN,
    ERROR_LET_TOKEN_IDENTIFIER,
    ERROR_TOKEN_ILLEGAL,
    ERROR_TOKEN_UNEXPECTED,
    ERROR_EXPRESSION_GROUP_EXPECTED_PAREN,
    ERROR_EXPRESSION_IF_EXPECTED_LEFT_PAREN,
    ERROR_EXPRESSION_IF_EXPECTED_RIGHT_PAREN,
    ERROR_EXPRESSION_BLOCK_EXPECTED_LEFT_BRACE,
    ERROR_EXPRESSION_BLOCK_EXPECTED_RIGHT_BRACE,
    ERROR_EXPRESSION_PUTS_EXPECTED_LEFT_PAREN,
    ERROR_EXPRESSION_PUTS_EXPECTED_RIGHT_PAREN,
};

typedef struct Error Error;
struct Error {
    Error* next;
    ErrorType type;
    Token token;
};

typedef struct ErrorList ErrorList;
struct ErrorList {
    Error* head;
    Error* tail;
};

void errors_free(ErrorList*);
void errors_append(ErrorList*, ErrorType, const Token*);
void errors_print(const ErrorList*);

#endif // MONKEY_ERROR_H_
