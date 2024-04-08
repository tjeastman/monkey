#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "monkey/error.h"
#include "monkey/expression.h"
#include "monkey/parser.h"
#include "monkey/statement.h"
#include "monkey/string.h"
#include "monkey/token.h"

void parser_init(Parser* parser, FILE* file)
{
    lexer_init(&parser->lexer, file);
    token_init(&parser->token, parser->lexer.line, parser->lexer.position);
    parser->token_next = lexer_token_next(&parser->lexer);
    error_init(&parser->error);
}

void parser_free(Parser* parser)
{
    token_free(&parser->token);
    error_free(&parser->error);
}

bool parser_next(Parser* parser)
{
    token_free(&parser->token);
    parser->token = parser->token_next;
    parser->token_next = lexer_token_next(&parser->lexer);
    return parser->token.type != TOKEN_END && parser->token.type != TOKEN_ILLEGAL;
}

void parser_error(Parser* parser, ErrorType type)
{
    parser->error.type = type;
    token_copy(&parser->error.token, &parser->token);
}

bool parser_parse_prefix_expression(Parser* parser, Expression* expression, Operation operation)
{
    expression_init_prefix(expression, operation);

    parser_next(parser);
    return parser_parse_expression(parser, expression->prefix.operand, PRECEDENCE_PREFIX);
}

bool parser_parse_grouped_expression(Parser* parser, Expression* expression)
{
    parser_next(parser);
    if (!parser_parse_expression(parser, expression, PRECEDENCE_LOWEST)) {
        return false;
    } else if (parser->token_next.type != TOKEN_RIGHT_PAREN) {
        parser_error(parser, ERROR_EXPRESSION_GROUP_EXPECTED_PAREN);
        return false;
    }
    parser_next(parser);
    return true;
}

bool parser_parse_block_expression(Parser* parser, StatementBlock* block)
{
    parser_next(parser);
    if (parser->token.type != TOKEN_LEFT_BRACE) {
        parser_error(parser, ERROR_EXPRESSION_BLOCK_EXPECTED_LEFT_BRACE);
        return false;
    }

    parser_next(parser);
    while (parser->token.type != TOKEN_RIGHT_BRACE) {
        Statement statement;
        statement_init(&statement);
        if (!parser_parse_statement(parser, &statement)) {
            statement_free(&statement);
            return false;
        }
        statement_block_extend(block, &statement);
        parser_next(parser);
    }

    return true;
}

bool parser_parse_conditional_expression(Parser* parser, Expression* expression)
{
    parser_next(parser);
    if (parser->token.type != TOKEN_LEFT_PAREN) {
        parser_error(parser, ERROR_EXPRESSION_IF_EXPECTED_LEFT_PAREN);
        return false;
    }

    expression_init_conditional(expression);

    parser_next(parser);
    if (!parser_parse_expression(parser, expression->conditional.condition, PRECEDENCE_LOWEST)) {
        return false;
    }

    parser_next(parser);
    if (parser->token.type != TOKEN_RIGHT_PAREN) {
        parser_error(parser, ERROR_EXPRESSION_IF_EXPECTED_RIGHT_PAREN);
        return false;
    }

    expression->conditional.consequence = (StatementBlock*)malloc(sizeof(StatementBlock));
    statement_block_init(expression->conditional.consequence);

    if (!parser_parse_block_expression(parser, expression->conditional.consequence)) {
        return false;
    } else if (parser->token_next.type != TOKEN_ELSE) {
        return true;
    }
    parser_next(parser);

    expression->conditional.alternate = (StatementBlock*)malloc(sizeof(StatementBlock));
    statement_block_init(expression->conditional.alternate);

    return parser_parse_block_expression(parser, expression->conditional.alternate);
}

bool parser_parse_function_expression_parameters(Parser* parser, FunctionParameter** parameters)
{
    FunctionParameter* parameter = NULL;

    parser_next(parser);
    while (parser->token.type != TOKEN_RIGHT_PAREN) {
        if (parser->token.type != TOKEN_IDENTIFIER) {
            parser_error(parser, ERROR_EXPRESSION_FUNCTION_EXPECTED_IDENTIFIER);
            return false;
        }

        if (parameter == NULL) {
            parameter = (FunctionParameter*)malloc(sizeof(FunctionParameter));
            *parameters = parameter;
        } else {
            parameter->next = (FunctionParameter*)malloc(sizeof(FunctionParameter));
            parameter = parameter->next;
        }
        string_copy(&parameter->name, &parser->token.lexeme);
        parameter->next = NULL;

        parser_next(parser);
        if (parser->token.type == TOKEN_RIGHT_PAREN) {
            break;
        } else if (parser->token.type == TOKEN_COMMA) {
            parser_next(parser);
        } else {
            parser_error(parser, ERROR_EXPRESSION_FUNCTION_EXPECTED_COMMA);
            return false;
        }
    }

    return true;
}

bool parser_parse_function_expression(Parser* parser, Expression* expression)
{
    parser_next(parser);
    if (parser->token.type != TOKEN_LEFT_PAREN) {
        parser_error(parser, ERROR_EXPRESSION_FUNCTION_EXPECTED_LEFT_PAREN);
        return false;
    }

    expression_init_function(expression);

    if (!parser_parse_function_expression_parameters(parser, &expression->function.parameters)) {
        return false;
    }

    expression->function.body = (StatementBlock*)malloc(sizeof(StatementBlock));
    statement_block_init(expression->function.body);

    return parser_parse_block_expression(parser, expression->function.body);
}

bool parser_parse_call_expression_arguments(Parser* parser, Expression* expression, FunctionArgument** arguments)
{
    FunctionArgument* argument = NULL;

    parser_next(parser);
    while (parser->token.type != TOKEN_RIGHT_PAREN) {
        if (argument == NULL) {
            argument = (FunctionArgument*)malloc(sizeof(FunctionArgument));
            *arguments = argument;
        } else {
            argument->next = (FunctionArgument*)malloc(sizeof(FunctionArgument));
            argument = argument->next;
        }
        argument->next = NULL;

        argument->expression = expression_new();
        if (!parser_parse_expression(parser, argument->expression, PRECEDENCE_LOWEST)) {
            return false;
        }

        parser_next(parser);
        if (parser->token.type == TOKEN_RIGHT_PAREN) {
            break;
        } else if (parser->token.type == TOKEN_COMMA) {
            parser_next(parser);
        } else {
            parser_error(parser, ERROR_EXPRESSION_CALL_EXPECTED_COMMA);
            return false;
        }
    }

    return true;
}

bool parser_parse_call_expression(Parser* parser, Expression* expression)
{
    if (!expression_init_call(expression, expression_move(expression))) {
        return false;
    }
    return parser_parse_call_expression_arguments(parser, expression, &expression->call.arguments);
}

bool parser_parse_puts_expression(Parser* parser, Expression* expression)
{
    parser_next(parser);
    if (parser->token.type != TOKEN_LEFT_PAREN) {
        parser_error(parser, ERROR_EXPRESSION_PUTS_EXPECTED_LEFT_PAREN);
        return false;
    }

    expression->type = EXPRESSION_PUTS;
    expression->puts.expression = expression_new();

    parser_next(parser);
    if (!parser_parse_expression(parser, expression->puts.expression, PRECEDENCE_LOWEST)) {
        return false;
    }

    parser_next(parser);
    if (parser->token.type != TOKEN_RIGHT_PAREN) {
        parser_error(parser, ERROR_EXPRESSION_PUTS_EXPECTED_RIGHT_PAREN);
        return false;
    }

    return true;
}

bool parser_parse_expression_left(Parser* parser, Expression* expression)
{
    switch (parser->token.type) {
    case TOKEN_IDENTIFIER:
        return expression_init_identifier(expression, &parser->token.lexeme);
    case TOKEN_INTEGER:
        return expression_init_integer(expression, atoi(parser->token.lexeme.value));
    case TOKEN_STRING:
        return expression_init_string(expression, &parser->token.lexeme);
    case TOKEN_TRUE:
    case TOKEN_FALSE:
        return expression_init_bool(expression, parser->token.type == TOKEN_TRUE);
    case TOKEN_MINUS:
        return parser_parse_prefix_expression(parser, expression, OPERATION_NEGATIVE);
    case TOKEN_NOT:
        return parser_parse_prefix_expression(parser, expression, OPERATION_NOT);
    case TOKEN_LEFT_PAREN:
        return parser_parse_grouped_expression(parser, expression);
    case TOKEN_IF:
        return parser_parse_conditional_expression(parser, expression);
    case TOKEN_FUNCTION:
        return parser_parse_function_expression(parser, expression);
    case TOKEN_PUTS:
        return parser_parse_puts_expression(parser, expression);
    default:
        parser_error(parser, ERROR_TOKEN_UNEXPECTED);
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

        if (!expression_init_infix(expression, expression_move(expression), operation)) {
            return false;
        }

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

    if (parser->token_next.type == TOKEN_LEFT_PAREN) {
        parser_next(parser);
        if (!parser_parse_call_expression(parser, expression)) {
            return false;
        }
    }
    return true;
}

bool parser_parse_let_statement(Parser* parser, Statement* statement)
{
    parser_next(parser);
    if (parser->token.type != TOKEN_IDENTIFIER) {
        parser_error(parser, ERROR_LET_TOKEN_IDENTIFIER);
        return false;
    }

    statement->identifier = (String*)malloc(sizeof(String));
    string_copy(statement->identifier, &parser->token.lexeme);

    parser_next(parser);
    if (parser->token.type != TOKEN_ASSIGN) {
        parser_error(parser, ERROR_LET_TOKEN_ASSIGN);
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

bool parser_parse_program(Parser* parser, StatementBlock* block)
{
    while (parser_next(parser)) {
        Statement statement;
        statement_init(&statement);
        if (parser_parse_statement(parser, &statement)) {
            statement_block_extend(block, &statement);
        } else {
            statement_free(&statement);
            return false;
        }
    }
    if (parser->token.type == TOKEN_ILLEGAL) {
        parser_error(parser, ERROR_TOKEN_ILLEGAL);
        return false;
    }
    return true;
}
