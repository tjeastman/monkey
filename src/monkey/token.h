#ifndef MONKEY_TOKEN_H_
#define MONKEY_TOKEN_H_

#include <stddef.h>

#include "monkey/string.h"

typedef enum TokenType TokenType;
enum TokenType {
    TOKEN_NONE,
    TOKEN_END,
    TOKEN_ILLEGAL,
    TOKEN_COMMENT,
    TOKEN_IDENTIFIER,
    // keywords
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FUNCTION,
    TOKEN_RETURN,
    TOKEN_MACRO,
    TOKEN_QUOTE,
    TOKEN_UNQUOTE,
    // literals
    TOKEN_INTEGER,
    TOKEN_STRING,
    TOKEN_TRUE,
    TOKEN_FALSE,
    // operators
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_NOT,
    TOKEN_NOT_EQUAL,
    // delimiters
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_LEFT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_PAREN,
    TOKEN_RIGHT_BRACE,
    TOKEN_RIGHT_BRACKET,
};

typedef struct Token Token;
struct Token {
    TokenType type;
    size_t line;
    ssize_t position;
    String lexeme;
};

void token_init(Token*, size_t, ssize_t);
void token_free(const Token*);
void token_reset(Token*);
void token_append(Token*, char);
void token_copy(Token*, const Token*);
void token_print(const Token*);

#endif // MONKEY_TOKEN_H_
