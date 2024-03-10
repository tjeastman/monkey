#ifndef MONKEY_LEXER_H_
#define MONKEY_LEXER_H_

#include <stdbool.h>
#include <stdio.h>

#include "monkey/token.h"

typedef struct Lexer Lexer;
struct Lexer {
    FILE* file;
    size_t line;
    ssize_t position;
    bool pushed;
    char push;
};

void lexer_init(Lexer*, FILE*);
Token lexer_token_next(Lexer*);

#endif // MONKEY_LEXER_H_
