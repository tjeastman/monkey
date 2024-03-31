#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "monkey/lexer.h"
#include "monkey/token.h"

void lexer_init(Lexer* lexer, FILE* file)
{
    lexer->file = file;
    lexer->line = 0;
    lexer->position = 0;
    lexer->pushed = false;
}

int lexer_pop_char(Lexer* lexer)
{
    if (lexer->pushed) {
        lexer->pushed = false;
        return lexer->push;
    }

    int c = fgetc(lexer->file);
    if (c == '\n') {
        lexer->line++;
        lexer->position = -1; // next char after newline is 0
    } else {
        lexer->position += 1;
    }
    return c;
}

void lexer_push_char(Lexer* lexer, int c)
{
    lexer->pushed = true;
    lexer->push = c;
}

void lexer_token_start(Lexer* lexer, Token* token, TokenType type)
{
    token->type = type;
    token->line = lexer->line;
    token->position = lexer->position;
}

bool lexer_token_next_aux(Lexer* lexer, Token* token)
{
    int c;
next:
    /*
     * Some tokens span multiple input characters. The lexer must "look ahead"
     * to subsequent input characters to determine if the end of the token has
     * occurred. Handle the case where tokens in this class are terminated by
     * reaching the end of the input file.
     */
    if ((c = lexer_pop_char(lexer)) != EOF) {

    } else if (token->type == TOKEN_NONE || token->type == TOKEN_COMMENT) {
        lexer_token_start(lexer, token, TOKEN_END);
        return true;
    } else if (token->type == TOKEN_IDENTIFIER) {
        return true;
    } else if (token->type == TOKEN_INTEGER) {
        return true;
    } else if (token->type == TOKEN_ASSIGN) {
        return true;
    } else if (token->type == TOKEN_DIVIDE) {
        return true;
    } else if (token->type == TOKEN_GREATER) {
        return true;
    } else if (token->type == TOKEN_LESS) {
        return true;
    } else {
        token->type = TOKEN_ILLEGAL;
        return false;
    }

    /*
     * Discard whitespace characters that occur outside the context of an
     * established token type.
     */
    if (token->type == TOKEN_NONE && isspace(c)) {
        goto next;
    }

    /*
     * Check for the quote character starting a string token before extending
     * the current lexeme. Note that the terminating quote character is excluded
     * from the lexeme below.
     */
    if (token->type == TOKEN_NONE && c == '"') {
        lexer_token_start(lexer, token, TOKEN_STRING);
        goto next;
    }

    /*
     * Check for the termination of tokens that "look ahead" due to conditions
     * other than having reached the end of the input.
     *
     * Note that comments are excluded from the token stream of the lexer. Note
     * also that the terminal quote character of a string token is excluded from
     * the corresponding lexeme.
     */
    if (token->type == TOKEN_COMMENT && c == '\n') {
        token_reset(token);
        goto next;
    } else if (token->type == TOKEN_IDENTIFIER && !isalnum(c) && c != '_') {
        lexer_push_char(lexer, c);
        return true;
    } else if (token->type == TOKEN_INTEGER && !isdigit(c)) {
        lexer_push_char(lexer, c);
        return true;
    } else if (token->type == TOKEN_STRING && c == '"') {
        return true;
    } else if (token->type == TOKEN_ASSIGN && c != '=') {
        lexer_push_char(lexer, c);
        return true;
    } else if (token->type == TOKEN_DIVIDE && c != '/') {
        lexer_push_char(lexer, c);
        return true;
    } else if (token->type == TOKEN_GREATER && c != '=') {
        lexer_push_char(lexer, c);
        return true;
    } else if (token->type == TOKEN_LESS && c != '=') {
        lexer_push_char(lexer, c);
        return true;
    } else if (token->type == TOKEN_NOT && c != '=') {
        lexer_push_char(lexer, c);
        return true;
    }

    token_append(token, c);

    /*
     * Process the next character for the "look ahead" tokens. Note that the
     * termination conditions for these tokens were all checked above. Here it
     * is safe to assume that the current character is a valid extension of the
     * corresponding lexeme. Note that some token lexemes are suffixes of others
     * so token types may change in some cases.
     */
    if (token->type == TOKEN_COMMENT) {
        goto next;
    } else if (token->type == TOKEN_IDENTIFIER) {
        goto next;
    } else if (token->type == TOKEN_INTEGER) {
        goto next;
    } else if (token->type == TOKEN_STRING) {
        goto next;
    } else if (token->type == TOKEN_ASSIGN) {
        token->type = TOKEN_EQUAL;
        return true;
    } else if (token->type == TOKEN_DIVIDE) {
        token->type = TOKEN_COMMENT;
        goto next;
    } else if (token->type == TOKEN_GREATER) {
        token->type = TOKEN_GREATER_EQUAL;
        return true;
    } else if (token->type == TOKEN_LESS) {
        token->type = TOKEN_LESS_EQUAL;
        return true;
    } else if (token->type == TOKEN_NOT) {
        token->type = TOKEN_NOT_EQUAL;
        return true;
    }

    /*
     * Handle the start of new tokens. The current character uniquely identifies
     * the possible token types that can follow. Note that language keywords are
     * indistinguishable from identifiers until the end of the token is reached.
     * Token processing treats all as identifiers and checks for matching
     * keywords just before returning.
     */
    if (isalpha(c)) {
        lexer_token_start(lexer, token, TOKEN_IDENTIFIER);
        goto next;
    } else if (isdigit(c)) {
        lexer_token_start(lexer, token, TOKEN_INTEGER);
        goto next;
    }

    switch (c) {
    case '=':
        lexer_token_start(lexer, token, TOKEN_ASSIGN);
        goto next;
    case '+':
        lexer_token_start(lexer, token, TOKEN_PLUS);
        return true;
    case '-':
        lexer_token_start(lexer, token, TOKEN_MINUS);
        return true;
    case '*':
        lexer_token_start(lexer, token, TOKEN_MULTIPLY);
        return true;
    case '/':
        lexer_token_start(lexer, token, TOKEN_DIVIDE);
        goto next;
    case '>':
        lexer_token_start(lexer, token, TOKEN_GREATER);
        goto next;
    case '<':
        lexer_token_start(lexer, token, TOKEN_LESS);
        goto next;
    case '!':
        lexer_token_start(lexer, token, TOKEN_NOT);
        goto next;
    case ',':
        lexer_token_start(lexer, token, TOKEN_COMMA);
        return true;
    case ':':
        lexer_token_start(lexer, token, TOKEN_COLON);
        return true;
    case ';':
        lexer_token_start(lexer, token, TOKEN_SEMICOLON);
        return true;
    case '(':
        lexer_token_start(lexer, token, TOKEN_LEFT_PAREN);
        return true;
    case '{':
        lexer_token_start(lexer, token, TOKEN_LEFT_BRACE);
        return true;
    case '[':
        lexer_token_start(lexer, token, TOKEN_LEFT_BRACKET);
        return true;
    case ')':
        lexer_token_start(lexer, token, TOKEN_RIGHT_PAREN);
        return true;
    case '}':
        lexer_token_start(lexer, token, TOKEN_RIGHT_BRACE);
        return true;
    case ']':
        lexer_token_start(lexer, token, TOKEN_RIGHT_BRACKET);
        return true;
    default:
        lexer_token_start(lexer, token, TOKEN_ILLEGAL);
        return false;
    }
}

Token lexer_token_next(Lexer* lexer)
{
    Token token;
    token_init(&token, lexer->line, lexer->position);

    if (feof(lexer->file)) {
        lexer_token_start(lexer, &token, TOKEN_END);
        return token;
    } else if (!lexer_token_next_aux(lexer, &token)) {
        return token;
    } else if (token.type != TOKEN_IDENTIFIER) {
        return token;
    }

    /*
     * Compare identifier tokens to language keywords and convert to appropriate
     * keyword tokens if a match is found.
     */
    if (strncmp(token.lexeme.value, "let", 3) == 0) {
        token.type = TOKEN_LET;
    } else if (strncmp(token.lexeme.value, "if", 2) == 0) {
        token.type = TOKEN_IF;
    } else if (strncmp(token.lexeme.value, "else", 4) == 0) {
        token.type = TOKEN_ELSE;
    } else if (strncmp(token.lexeme.value, "fn", 2) == 0) {
        token.type = TOKEN_FUNCTION;
    } else if (strncmp(token.lexeme.value, "return", 6) == 0) {
        token.type = TOKEN_RETURN;
    } else if (strncmp(token.lexeme.value, "true", 4) == 0) {
        token.type = TOKEN_TRUE;
    } else if (strncmp(token.lexeme.value, "false", 5) == 0) {
        token.type = TOKEN_FALSE;
    } else if (strncmp(token.lexeme.value, "puts", 4) == 0) {
        token.type = TOKEN_PUTS;
    }
    return token;
}
