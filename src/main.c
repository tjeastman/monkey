#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/error.h"
#include "monkey/eval.h"
#include "monkey/lexer.h"
#include "monkey/parser.h"

bool tokenize(FILE* file)
{
    Lexer lexer;
    lexer_init(&lexer, file);
    bool stop = false;
    while (!stop) {
        Token token = lexer_token_next(&lexer);
        if (token.type == TOKEN_END) {
            stop = true;
        } else if (token.type == TOKEN_ILLEGAL) {
            stop = true;
            printf("unrecognized token: %s\n", token.lexeme.value);
        } else {
            token_print(&token);
        }
        token_free(&token);
    }
    return true;
}

bool parse(FILE* file)
{
    Parser parser;
    parser_init(&parser, file);

    StatementBlock block;
    statement_block_init(&block);

    bool result = parser_parse_program(&parser, &block);
    if (result) {
        statement_block_print(&block, 0);
    } else {
        error_print(&parser.error);
    }

    statement_block_free(&block);
    parser_free(&parser);

    return result;
}

bool eval(FILE* file)
{
    Parser parser;
    parser_init(&parser, file);

    StatementBlock block;
    statement_block_init(&block);

    bool result = parser_parse_program(&parser, &block);
    if (result) {
        evaluate_program(&block);
    } else {
        error_print(&parser.error);
    }

    statement_block_free(&block);
    parser_free(&parser);

    return result;
}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s command file\n", argv[0]);
        exit(1);
    }

    FILE* file = fopen(argv[2], "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", argv[2]);
        exit(1);
    }

    bool result = true;
    if (strncmp(argv[1], "tokenize", 8) == 0) {
        result = tokenize(file);
    } else if (strncmp(argv[1], "parse", 5) == 0) {
        result = parse(file);
    } else if (strncmp(argv[1], "eval", 4) == 0) {
        result = eval(file);
    } else {
        printf("Unrecognized command: %s\n", argv[1]);
        exit(1);
    }

    if (result) {
        return 0;
    } else {
        return 1;
    }
}
