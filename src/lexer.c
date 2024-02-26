#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "monkey/lexer.h"
#include "monkey/token.h"

bool lexer_next_aux(FILE* file, Token* token)
{
    int c;
next:
    /*
     * Some tokens span multiple input characters. The lexer must "look ahead"
     * to subsequent input characters to determine if the end of the token has
     * occurred. Handle the case where tokens in this class are terminated by
     * reaching the end of the input file.
     */
    if ((c = fgetc(file)) != EOF) {

    } else if (token->type == TOKEN_NONE || token->type == TOKEN_COMMENT) {
        token->type = TOKEN_END;
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
        token->type = TOKEN_STRING;
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
        ungetc(c, file);
        return true;
    } else if (token->type == TOKEN_INTEGER && !isdigit(c)) {
        ungetc(c, file);
        return true;
    } else if (token->type == TOKEN_STRING && c == '"') {
        return true;
    } else if (token->type == TOKEN_ASSIGN && c != '=') {
        ungetc(c, file);
        return true;
    } else if (token->type == TOKEN_DIVIDE && c != '/') {
        ungetc(c, file);
        return true;
    } else if (token->type == TOKEN_GREATER && c != '=') {
        ungetc(c, file);
        return true;
    } else if (token->type == TOKEN_LESS && c != '=') {
        ungetc(c, file);
        return true;
    } else if (token->type == TOKEN_NOT && c != '=') {
        ungetc(c, file);
        return true;
    }

    token_extend(token, c);

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
        token->type = TOKEN_IDENTIFIER;
        goto next;
    } else if (isdigit(c)) {
        token->type = TOKEN_INTEGER;
        goto next;
    }

    switch (c) {
    case '=':
        token->type = TOKEN_ASSIGN;
        goto next;
    case '+':
        token->type = TOKEN_PLUS;
        return true;
    case '-':
        token->type = TOKEN_MINUS;
        return true;
    case '*':
        token->type = TOKEN_MULTIPLY;
        return true;
    case '/':
        token->type = TOKEN_DIVIDE;
        goto next;
    case '>':
        token->type = TOKEN_GREATER;
        goto next;
    case '<':
        token->type = TOKEN_LESS;
        goto next;
    case '!':
        token->type = TOKEN_NOT;
        goto next;
    case ',':
        token->type = TOKEN_COMMA;
        return true;
    case ':':
        token->type = TOKEN_COLON;
        return true;
    case ';':
        token->type = TOKEN_SEMICOLON;
        return true;
    case '(':
        token->type = TOKEN_LEFT_PAREN;
        return true;
    case '{':
        token->type = TOKEN_LEFT_BRACE;
        return true;
    case '[':
        token->type = TOKEN_LEFT_BRACKET;
        return true;
    case ')':
        token->type = TOKEN_RIGHT_PAREN;
        return true;
    case '}':
        token->type = TOKEN_RIGHT_BRACE;
        return true;
    case ']':
        token->type = TOKEN_RIGHT_BRACKET;
        return true;
    default:
        token->type = TOKEN_ILLEGAL;
        return false;
    }
}

bool lexer_next(FILE* file, Token* token)
{
    token_reset(token);

    if (feof(file)) {
        return false;
    } else if (!lexer_next_aux(file, token)) {
        return false;
    } else if (token->type != TOKEN_IDENTIFIER) {
        return true;
    }

    /*
     * Compare identifier tokens to language keywords and convert to appropriate
     * keyword tokens if a match is found.
     */
    if (strncmp(token->lexeme, "let", 3) == 0) {
        token->type = TOKEN_LET;
    } else if (strncmp(token->lexeme, "if", 2) == 0) {
        token->type = TOKEN_IF;
    } else if (strncmp(token->lexeme, "else", 4) == 0) {
        token->type = TOKEN_ELSE;
    } else if (strncmp(token->lexeme, "fn", 2) == 0) {
        token->type = TOKEN_FUNCTION;
    } else if (strncmp(token->lexeme, "return", 6) == 0) {
        token->type = TOKEN_RETURN;
    } else if (strncmp(token->lexeme, "true", 4) == 0) {
        token->type = TOKEN_TRUE;
    } else if (strncmp(token->lexeme, "false", 5) == 0) {
        token->type = TOKEN_FALSE;
    }
    return true;
}
