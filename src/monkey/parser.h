#ifndef MONKEY_PARSER_H_
#define MONKEY_PARSER_H_

#include <stdbool.h>
#include <stdio.h>

#include "monkey/error.h"
#include "monkey/lexer.h"
#include "monkey/program.h"
#include "monkey/token.h"

typedef struct Parser Parser;
struct Parser {
    Lexer lexer;
    Token token;
    Token token_next;
    ErrorList errors;
};

void parser_init(Parser*, FILE*);
void parser_free(Parser*);
bool parser_parse_program(Parser*, Program*);

#endif // MONKEY_PARSER_H_
