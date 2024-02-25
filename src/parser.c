#include <stdbool.h>
#include <stdlib.h>

#include "monkey/expression.h"
#include "monkey/lexer.h"
#include "monkey/parser.h"
#include "monkey/program.h"
#include "monkey/string.h"
#include "monkey/token.h"

Parser* parser_init(FILE* file)
{
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    parser->file = file;
    parser->token = token_init();
    parser->token_next = token_init();
    lexer_next(file, parser->token);
    lexer_next(file, parser->token_next);
    return parser;
}

void parser_free(Parser* parser)
{
    token_free(parser->token);
    token_free(parser->token_next);
    free(parser);
}

void parser_next(Parser* parser)
{
    token_copy(parser->token, parser->token_next);
    lexer_next(parser->file, parser->token_next);
}

Expression* parser_parse_identifier(Parser* parser)
{
    Expression* expression = expression_init(EXPRESSION_IDENTIFIER);
    expression->identifier.value = string_copy(parser->token->lexeme);
    return expression;
}

Expression* parser_parse_integer(Parser* parser)
{
    Expression* expression = expression_init(EXPRESSION_INTEGER);
    expression->integer.value = atoi(parser->token->lexeme);
    return expression;
}

Expression* parser_parse_string(Parser* parser)
{
    Expression* expression = expression_init(EXPRESSION_STRING);
    expression->string.value = string_copy(parser->token->lexeme);
    return expression;
}

Expression* parser_parse_bool(Parser* parser)
{
    Expression* expression = expression_init(EXPRESSION_BOOL);
    expression->boolean.value = parser->token->type == TOKEN_TRUE;
    return expression;
}

Expression* parser_parse_expression(Parser*, Precedence);

Expression* parser_parse_prefix_expression(Parser* parser, Operation operation)
{
    parser_next(parser);
    Expression* expression = expression_init(EXPRESSION_PREFIX);
    expression->prefix.operation = operation;
    expression->prefix.operand = parser_parse_expression(parser, PRECEDENCE_PREFIX);
    return expression;
}

Expression* parser_parse_expression_left(Parser* parser)
{
    switch (parser->token->type) {
    case TOKEN_IDENTIFIER:
        return parser_parse_identifier(parser);
    case TOKEN_INTEGER:
        return parser_parse_integer(parser);
    case TOKEN_STRING:
        return parser_parse_string(parser);
    case TOKEN_TRUE:
    case TOKEN_FALSE:
        return parser_parse_bool(parser);
    case TOKEN_MINUS:
        return parser_parse_prefix_expression(parser, OPERATION_NEGATIVE);
    case TOKEN_NOT:
        return parser_parse_prefix_expression(parser, OPERATION_NOT);
    default:
        return NULL;
    }
}

bool parser_parse_infix_operation(Parser* parser, Operation* operation)
{
    switch (parser->token_next->type) {
    case TOKEN_EQUAL:
        *operation = OPERATION_EQUAL;
        break;
    case TOKEN_NOT_EQUAL:
        *operation = OPERATION_NOT_EQUAL;
        break;
    case TOKEN_LESS:
        *operation = OPERATION_LESS;
        break;
    case TOKEN_LESS_EQUAL:
        *operation = OPERATION_LESS_EQUAL;
        break;
    case TOKEN_GREATER:
        *operation = OPERATION_GREATER;
        break;
    case TOKEN_GREATER_EQUAL:
        *operation = OPERATION_GREATER_EQUAL;
        break;
    case TOKEN_PLUS:
        *operation = OPERATION_ADD;
        break;
    case TOKEN_MINUS:
        *operation = OPERATION_SUBTRACT;
        break;
    case TOKEN_DIVIDE:
        *operation = OPERATION_DIVIDE;
        break;
    case TOKEN_MULTIPLY:
        *operation = OPERATION_MULTIPLY;
        break;
    default:
        return false;
    }
    return true;
}

Expression* parser_parse_expression_right(Parser* parser, Expression* expression_left, Precedence precedence_min)
{
    while (true) {
        Operation operation;
        if (!parser_parse_infix_operation(parser, &operation)) {
            return expression_left;
        }

        Precedence precedence = operation_precedence(operation);
        if (precedence <= precedence_min) {
            return expression_left;
        }

        parser_next(parser);
        parser_next(parser);
        Expression* expression_right = parser_parse_expression(parser, precedence);
        if (expression_right == NULL) {
            expression_free(expression_left);
            return NULL;
        }

        Expression* expression = expression_init(EXPRESSION_INFIX);
        expression->infix.operand[0] = expression_left;
        expression->infix.operand[1] = expression_right;
        expression->infix.operation = operation;
        expression_left = expression;
    }
}

Expression* parser_parse_expression(Parser* parser, Precedence precedence)
{
    Expression* expression;
    expression = parser_parse_expression_left(parser);
    if (expression == NULL) {
        return NULL;
    }
    expression = parser_parse_expression_right(parser, expression, precedence);
    if (expression == NULL) {
        return NULL;
    }
    return expression;
}

Statement* parser_parse_let_statement(Parser* parser)
{
    parser_next(parser);
    if (parser->token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }

    char* identifier = string_copy(parser->token->lexeme);

    parser_next(parser);
    if (parser->token->type != TOKEN_ASSIGN) {
        free(identifier);
        return NULL;
    }

    parser_next(parser);
    Expression* expression = parser_parse_expression(parser, PRECEDENCE_LOWEST);
    if (expression == NULL) {
        free(identifier);
        return NULL;
    }
    if (parser->token_next->type == TOKEN_SEMICOLON) {
        parser_next(parser);
    }

    return statement_init(STATEMENT_LET, identifier, expression);
}

Statement* parser_parse_return_statement(Parser* parser)
{
    parser_next(parser);
    Expression* expression = parser_parse_expression(parser, PRECEDENCE_LOWEST);
    if (expression == NULL) {
        return NULL;
    }
    if (parser->token_next->type == TOKEN_SEMICOLON) {
        parser_next(parser);
    }
    return statement_init(STATEMENT_RETURN, NULL, expression);
}

Statement* parser_parse_expression_statement(Parser* parser)
{
    Expression* expression = parser_parse_expression(parser, PRECEDENCE_LOWEST);
    if (expression == NULL) {
        return NULL;
    }
    if (parser->token_next->type == TOKEN_SEMICOLON) {
        parser_next(parser);
    }
    return statement_init(STATEMENT_EXPRESSION, NULL, expression);
}

Statement* parser_parse_statement(Parser* parser)
{
    switch (parser->token->type) {
    case TOKEN_LET:
        return parser_parse_let_statement(parser);
    case TOKEN_RETURN:
        return parser_parse_return_statement(parser);
    default:
        return parser_parse_expression_statement(parser);
    }
}

bool parser_parse_program(Parser* parser, Program* program)
{
    while (parser->token->type != TOKEN_END) {
        Statement* statement = parser_parse_statement(parser);
        if (statement == NULL) {
            return false;
        }
        program_extend(program, statement);
        parser_next(parser);
    }
    return true;
}
