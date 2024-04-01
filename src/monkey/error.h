#ifndef MONKEY_ERROR_H_
#define MONKEY_ERROR_H_

#include <stddef.h>

#include "monkey/token.h"

typedef enum ErrorType ErrorType;
enum ErrorType {
    ERROR_NONE,
    ERROR_LET_TOKEN_ASSIGN,
    ERROR_LET_TOKEN_IDENTIFIER,
    ERROR_TOKEN_ILLEGAL,
    ERROR_TOKEN_UNEXPECTED,
    ERROR_EXPRESSION_GROUP_EXPECTED_PAREN,
    ERROR_EXPRESSION_IF_EXPECTED_LEFT_PAREN,
    ERROR_EXPRESSION_IF_EXPECTED_RIGHT_PAREN,
    ERROR_EXPRESSION_BLOCK_EXPECTED_LEFT_BRACE,
    ERROR_EXPRESSION_FUNCTION_EXPECTED_LEFT_PAREN,
    ERROR_EXPRESSION_FUNCTION_EXPECTED_IDENTIFIER,
    ERROR_EXPRESSION_FUNCTION_EXPECTED_COMMA,
    ERROR_EXPRESSION_CALL_EXPECTED_COMMA,
};

typedef struct Error Error;
struct Error {
    Error* next;
    ErrorType type;
    Token token;
};

void error_init(Error*);
void error_free(const Error*);
void error_print(const Error*);

#endif // MONKEY_ERROR_H_
