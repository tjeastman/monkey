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
void errors_append(ErrorList*, ErrorType, Token*);
void errors_print(ErrorList*);

#endif // MONKEY_ERROR_H_
