#include <stdbool.h>
#include <string.h>

#include "monkey/expression.h"
#include "monkey/parser.h"
#include "monkey/program.h"
#include "monkey/statement.h"
#include "monkey/string.h"

void parser_init(Parser* parser, FILE* file)
{
    lexer_init(&parser->lexer, file);
    token_init(&parser->token);
    parser->token_next = lexer_token_next(&parser->lexer);
    parser->errors.head = NULL;
    parser->errors.tail = NULL;
}

void parser_free(Parser* parser)
{
    token_free(&parser->token);
    errors_free(&parser->errors);
}

bool parser_next(Parser* parser)
{
    token_free(&parser->token);
    parser->token = parser->token_next;
    parser->token_next = lexer_token_next(&parser->lexer);
    return parser->token.type != TOKEN_END && parser->token.type != TOKEN_ILLEGAL;
}

bool parser_parse_integer(Parser* parser, Expression* expression)
{
    expression->type = EXPRESSION_INTEGER;
    expression->integer.value = atoi(parser->token.lexeme.value);
    return true;
}

bool parser_parse_bool(Parser* parser, Expression* expression)
{
    expression->type = EXPRESSION_BOOL;
    expression->boolean.value = parser->token.type == TOKEN_TRUE;
    return true;
}

bool parser_parse_string(Parser* parser, Expression* expression)
{
    expression->type = EXPRESSION_STRING;
    string_copy(&expression->string.value, &parser->token.lexeme);
    return true;
}

bool parser_parse_identifier(Parser* parser, Expression* expression)
{
    expression->type = EXPRESSION_IDENTIFIER;
    string_copy(&expression->identifier.value, &parser->token.lexeme);
    return true;
}

bool parser_parse_expression(Parser*, Expression*, Precedence);

bool parser_parse_prefix_expression(Parser* parser, Expression* expression, Operation operation)
{
    expression->type = EXPRESSION_PREFIX;
    expression->prefix.operation = operation;
    expression->prefix.operand = (Expression*)malloc(sizeof(Expression));
    expression->prefix.operand->type = EXPRESSION_NONE;

    parser_next(parser);
    if (!parser_parse_expression(parser, expression->prefix.operand, PRECEDENCE_PREFIX)) {
        return false;
    }
    return true;
}

bool parser_parse_expression_left(Parser* parser, Expression* expression)
{
    switch (parser->token.type) {
    case TOKEN_IDENTIFIER:
        return parser_parse_identifier(parser, expression);
    case TOKEN_INTEGER:
        return parser_parse_integer(parser, expression);
    case TOKEN_STRING:
        return parser_parse_string(parser, expression);
    case TOKEN_TRUE:
    case TOKEN_FALSE:
        return parser_parse_bool(parser, expression);
    case TOKEN_MINUS:
        return parser_parse_prefix_expression(parser, expression, OPERATION_NEGATIVE);
    case TOKEN_NOT:
        return parser_parse_prefix_expression(parser, expression, OPERATION_NOT);
    default:
        errors_append(&parser->errors, ERROR_TOKEN_UNEXPECTED, &parser->token);
        return false;
    }
}

Operation parser_parse_infix_operation(Parser* parser)
{
    switch (parser->token_next.type) {
    case TOKEN_EQUAL:
        return OPERATION_EQUAL;
    case TOKEN_NOT_EQUAL:
        return OPERATION_NOT_EQUAL;
    case TOKEN_LESS:
        return OPERATION_LESS;
    case TOKEN_LESS_EQUAL:
        return OPERATION_LESS_EQUAL;
    case TOKEN_GREATER:
        return OPERATION_GREATER;
    case TOKEN_GREATER_EQUAL:
        return OPERATION_GREATER_EQUAL;
    case TOKEN_PLUS:
        return OPERATION_ADD;
    case TOKEN_MINUS:
        return OPERATION_SUBTRACT;
    case TOKEN_DIVIDE:
        return OPERATION_DIVIDE;
    case TOKEN_MULTIPLY:
        return OPERATION_MULTIPLY;
    default:
        return OPERATION_NONE;
    }
}

bool parser_parse_expression_right(Parser* parser, Expression* expression, Precedence precedence_min)
{
    while (true) {
        Operation operation = parser_parse_infix_operation(parser);
        if (operation == OPERATION_NONE) {
            return true;
        }

        Precedence precedence = operation_precedence(operation);
        if (precedence <= precedence_min) {
            return true;
        }

        Expression* expression_left = (Expression*)malloc(sizeof(Expression));
        memcpy(expression_left, expression, sizeof(Expression));

        expression->type = EXPRESSION_INFIX;
        expression->infix.operand[0] = expression_left;
        expression->infix.operand[1] = (Expression*)malloc(sizeof(Expression));
        expression->infix.operand[1]->type = EXPRESSION_NONE;
        expression->infix.operation = operation;

        parser_next(parser);
        parser_next(parser);
        if (!parser_parse_expression(parser, expression->infix.operand[1], precedence)) {
            return false;
        }
    }
}

bool parser_parse_expression(Parser* parser, Expression* expression, Precedence precedence)
{
    if (!parser_parse_expression_left(parser, expression)) {
        return false;
    } else if (!parser_parse_expression_right(parser, expression, precedence)) {
        return false;
    }
    return true;
}

bool parser_parse_let_statement(Parser* parser, Statement* statement)
{
    parser_next(parser);
    if (parser->token.type != TOKEN_IDENTIFIER) {
        errors_append(&parser->errors, ERROR_LET_TOKEN_IDENTIFIER, &parser->token);
        return false;
    }

    statement->identifier = (String*)malloc(sizeof(String));
    string_copy(statement->identifier, &parser->token.lexeme);

    parser_next(parser);
    if (parser->token.type != TOKEN_ASSIGN) {
        errors_append(&parser->errors, ERROR_LET_TOKEN_ASSIGN, &parser->token);
        return false;
    }

    parser_next(parser);
    if (!parser_parse_expression(parser, &statement->expression, PRECEDENCE_LOWEST)) {
        return false;
    } else if (parser->token_next.type == TOKEN_SEMICOLON) {
        parser_next(parser);
    }
    return true;
}

bool parser_parse_return_statement(Parser* parser, Statement* statement)
{
    parser_next(parser);
    if (!parser_parse_expression(parser, &statement->expression, PRECEDENCE_LOWEST)) {
        return false;
    } else if (parser->token_next.type == TOKEN_SEMICOLON) {
        parser_next(parser);
    }
    return true;
}

bool parser_parse_expression_statement(Parser* parser, Statement* statement)
{
    if (!parser_parse_expression(parser, &statement->expression, PRECEDENCE_LOWEST)) {
        return false;
    } else if (parser->token_next.type == TOKEN_SEMICOLON) {
        parser_next(parser);
    }
    return true;
}

bool parser_parse_statement(Parser* parser, Statement* statement)
{
    if (parser->token.type == TOKEN_LET) {
        statement->type = STATEMENT_LET;
        return parser_parse_let_statement(parser, statement);
    } else if (parser->token.type == TOKEN_RETURN) {
        statement->type = STATEMENT_RETURN;
        return parser_parse_return_statement(parser, statement);
    } else {
        statement->type = STATEMENT_EXPRESSION;
        return parser_parse_expression_statement(parser, statement);
    }
}

bool parser_parse_program(Parser* parser, Program* program)
{
    while (parser_next(parser)) {
        Statement statement;
        statement_init(&statement);
        if (parser_parse_statement(parser, &statement)) {
            program_extend(program, statement);
        } else {
            statement_free(&statement);
        }
    }
    if (parser->token.type == TOKEN_ILLEGAL) {
        errors_append(&parser->errors, ERROR_TOKEN_ILLEGAL, &parser->token);
    }
    return parser->errors.head == NULL;
}
