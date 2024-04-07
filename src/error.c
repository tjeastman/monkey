#include <stdio.h>
#include <stdlib.h>

#include "monkey/error.h"
#include "monkey/token.h"

void error_init(Error* error)
{
    error->type = ERROR_NONE;
    token_init(&error->token, 0, 0);
}

void error_free(const Error* error)
{
    token_free(&error->token);
}

void error_print(const Error* error)
{
    printf("%zu:%zd: ", error->token.line, error->token.position);
    switch (error->type) {
    case ERROR_NONE:
        break;
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
    case ERROR_EXPRESSION_IF_EXPECTED_LEFT_PAREN:
        printf("expected opening left parenthesis in conditional expression\n");
        break;
    case ERROR_EXPRESSION_IF_EXPECTED_RIGHT_PAREN:
        printf("expected closing right parenthesis in conditional expression\n");
        break;
    case ERROR_EXPRESSION_BLOCK_EXPECTED_LEFT_BRACE:
        printf("expected opening left brace in conditional expression\n");
        break;
    case ERROR_EXPRESSION_PUTS_EXPECTED_LEFT_PAREN:
        printf("expected opening left parenthesis in puts expression\n");
        break;
    case ERROR_EXPRESSION_PUTS_EXPECTED_RIGHT_PAREN:
        printf("expected closing right parenthesis in puts expression\n");
        break;
    case ERROR_EXPRESSION_FUNCTION_EXPECTED_LEFT_PAREN:
        printf("expected left paren in function expression\n");
        break;
    case ERROR_EXPRESSION_FUNCTION_EXPECTED_IDENTIFIER:
        printf("expected identifier in function expression\n");
        break;
    case ERROR_EXPRESSION_FUNCTION_EXPECTED_COMMA:
        printf("expected comma in function parameters\n");
        break;
    case ERROR_EXPRESSION_CALL_EXPECTED_COMMA:
        printf("expected comma in function arguments\n");
        break;
    }
}
