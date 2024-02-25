#ifndef MONKEY_TOKEN_H_
#define MONKEY_TOKEN_H_

#include <stddef.h>

typedef enum {
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
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;
    size_t size;
    size_t position;
} Token;

Token* token_init();
void token_free(Token*);
void token_extend(Token*, char);
void token_reset(Token*);
void token_copy(Token*, Token*);
void token_print(Token*);

#endif // MONKEY_TOKEN_H_
