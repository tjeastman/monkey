#include <stdio.h>

#include "monkey/string.h"
#include "monkey/token.h"

void token_init(Token* token)
{
    token->type = TOKEN_NONE;
    token->line = 0;
    token->position = 0;
    string_init(&token->lexeme);
}

void token_free(Token* token)
{
    string_free(&token->lexeme);
}

void token_reset(Token* token)
{
    token->type = TOKEN_NONE;
    token->line = 0;
    token->position = 0;
    string_reset(&token->lexeme);
}

void token_copy(Token* destination, Token* source)
{
    destination->type = source->type;
    destination->line = source->line;
    destination->position = source->position;
    string_copy(&destination->lexeme, &source->lexeme);
}

void token_print(Token* token)
{
    switch (token->type) {
    case TOKEN_NONE:
        printf("NONE\n");
        break;
    case TOKEN_END:
        printf("END\n");
        break;
    case TOKEN_ILLEGAL:
        printf("ILLEGAL(%s)\n", token->lexeme.value);
        break;
    case TOKEN_COMMENT:
        printf("COMMENT(%s)\n", token->lexeme.value);
        break;
    case TOKEN_IDENTIFIER:
        printf("IDENTIFIER(%s)\n", token->lexeme.value);
        break;
    case TOKEN_LET:
        printf("LET\n");
        break;
    case TOKEN_IF:
        printf("IF\n");
        break;
    case TOKEN_ELSE:
        printf("ELSE\n");
        break;
    case TOKEN_FUNCTION:
        printf("FUNCTION\n");
        break;
    case TOKEN_RETURN:
        printf("RETURN\n");
        break;
    case TOKEN_INTEGER:
        printf("INTEGER(%s)\n", token->lexeme.value);
        break;
    case TOKEN_STRING:
        printf("STRING(%s)\n", token->lexeme.value);
        break;
    case TOKEN_TRUE:
        printf("TRUE\n");
        break;
    case TOKEN_FALSE:
        printf("FALSE\n");
        break;
    case TOKEN_ASSIGN:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_MULTIPLY:
    case TOKEN_DIVIDE:
    case TOKEN_EQUAL:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
    case TOKEN_NOT:
    case TOKEN_NOT_EQUAL:
        printf("OPERATOR[%s]\n", token->lexeme.value);
        break;
    case TOKEN_COMMA:
        printf("COMMA\n");
        break;
    case TOKEN_COLON:
        printf("COLON\n");
        break;
    case TOKEN_SEMICOLON:
        printf("SEMICOLON\n");
        break;
    case TOKEN_LEFT_PAREN:
        printf("LEFT_PAREN\n");
        break;
    case TOKEN_LEFT_BRACE:
        printf("LEFT_BRACE\n");
        break;
    case TOKEN_LEFT_BRACKET:
        printf("LEFT_BRACKET\n");
        break;
    case TOKEN_RIGHT_PAREN:
        printf("RIGHT_PAREN\n");
        break;
    case TOKEN_RIGHT_BRACE:
        printf("RIGHT_BRACE\n");
        break;
    case TOKEN_RIGHT_BRACKET:
        printf("RIGHT_BRACKET\n");
        break;
    }
}
