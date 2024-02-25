#ifndef MONKEY_PARSER_H_
#define MONKEY_PARSER_H_

#include <stdbool.h>
#include <stdio.h>

#include "monkey/expression.h"
#include "monkey/operation.h"
#include "monkey/program.h"
#include "monkey/statement.h"
#include "monkey/token.h"

typedef struct {
    FILE* file;
    Token* token;
    Token* token_next;
} Parser;

Parser* parser_init(FILE*);
void parser_free(Parser*);
bool parser_parse_program(Parser*, Program*);

#endif // MONKEY_PARSER_H_
