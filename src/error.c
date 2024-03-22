#include <stdio.h>
#include <stdlib.h>

#include "monkey/error.h"
#include "monkey/token.h"

void errors_free(ErrorList* errors)
{
    for (Error* error = errors->head; error != NULL; error = error->next) {
        token_free(&error->token);
    }
    errors->head = NULL;
    errors->tail = NULL;
}

void errors_append(ErrorList* errors, ErrorType type, Token* token)
{
    Error* error = (Error*)malloc(sizeof(Error));
    error->next = NULL;
    error->type = type;
    token_copy(&error->token, token);

    if (errors->head == NULL) {
        errors->head = error;
    }
    if (errors->tail == NULL) {
        errors->tail = error;
    } else {
        errors->tail->next = error;
        errors->tail = error;
    }
}

void error_print(Error* error)
{
    printf("%zu:%zd: ", error->token.line, error->token.position);
    switch (error->type) {
    case ERROR_LET_TOKEN_ASSIGN:
        printf("expected assignment op in let statement: %s", error->token.lexeme.value);
        break;
    case ERROR_LET_TOKEN_IDENTIFIER:
        printf("expected identifier in let statement: %s", error->token.lexeme.value);
        break;
    case ERROR_TOKEN_ILLEGAL:
        printf("unrecognized token: %s\n", error->token.lexeme.value);
        break;
    case ERROR_TOKEN_UNEXPECTED:
        printf("unexpected token: %s\n", error->token.lexeme.value);
        break;
    case ERROR_EXPRESSION_GROUP_EXPECTED_PAREN:
        printf("expected closing parenthesis in grouped expression\n");
        break;
    }
}

void errors_print(ErrorList* errors)
{
    for (Error* error = errors->head; error != NULL; error = error->next) {
        error_print(error);
    }
}
