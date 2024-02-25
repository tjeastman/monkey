#include <stdio.h>
#include <stdlib.h>

#include "monkey/lexer.h"
#include "monkey/parser.h"
#include "monkey/program.h"
#include "monkey/token.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage: %s file\n", argv[0]);
        exit(1);
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Error opening file: %s\n", argv[1]);
        exit(1);
    }

    Program* program = program_init();
    Parser* parser = parser_init(file);
    bool result = parser_parse_program(parser, program);
    parser_free(parser);
    program_print(program);
    program_free(program);

    if (result) {
        printf("result: true\n");
    } else {
        printf("result: false\n");
    }

    return 0;
}
